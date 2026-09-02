#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"
#include "clang/Tooling/Refactoring.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "llvm/Support/CommandLine.h"

#include <unordered_set>

#include "RefactorTool.h"

using namespace clang;
using namespace clang::ast_matchers;
using namespace clang::tooling;

static llvm::cl::OptionCategory ToolCategory("refactor-tool options");

// Метод run вызывается для каждого совпадения с матчем. 
// Мы проверяем тип совпадения по bind-именам и применяем рефакторинг.
void RefactorHandler::run(const MatchFinder::MatchResult &Result) {
    auto& Diag = Result.Context->getDiagnostics();
    auto& SM = *Result.SourceManager; // Получаем SourceManager для проверки isInMainFile
    
    if (const auto *Dtor = Result.Nodes.getNodeAs<CXXDestructorDecl>("nonVirtualDtor")) {
        handle_nv_dtor(Dtor, Diag, SM);
    }

    if (const auto *Method = Result.Nodes.getNodeAs<CXXMethodDecl>("missingOverride");
        Method && Method->size_overridden_methods() > 0 && !Method->hasAttr<OverrideAttr>()) {
        handle_miss_override(Method, Diag, SM);
    }

    if (const auto *LoopVar = Result.Nodes.getNodeAs<VarDecl>("loopVar")) {
        handle_crange_for(LoopVar, Diag, SM);
    }
}

void RefactorHandler::handle_nv_dtor(const CXXDestructorDecl *Dtor,
                            DiagnosticsEngine &Diag,
                            SourceManager &SM) {
    const auto source_loc = Dtor->getSourceRange().getBegin();
    if (SM.isInSystemHeader(source_loc) ||
        virtualDtorLocations.find(source_loc.getHashValue()) != virtualDtorLocations.end()
       )
        return;

    Rewrite.InsertTextBefore(source_loc, "virtual ");
    virtualDtorLocations.insert(source_loc.getHashValue());

    const unsigned DiagID = Diag.getCustomDiagID(
            DiagnosticsEngine::Remark,
            "Невиртуальный деструктор"
        );
    Diag.Report(Dtor->getLocation(), DiagID);
}

void RefactorHandler::handle_miss_override(const CXXMethodDecl *Method,
                            DiagnosticsEngine &Diag,
                            SourceManager &SM) {
    if (SM.isInSystemHeader(Method->getSourceRange().getBegin()))
        return;

    auto loc = Method->getNameInfo().getEndLoc();
    const auto LO = Rewrite.getLangOpts();

    while (true) {
        const auto token = Lexer::findNextToken(loc, SM, LO);

        if (!token)
            return;

        if (token->is(tok::r_paren)) {
            const auto ins_loc = Lexer::getLocForEndOfToken(
                token->getLocation(), 0, SM, LO
            );

            Rewrite.InsertText(ins_loc, " override");
            break;
        }

        loc = token->getLocation();
    }

    const unsigned DiagID = Diag.getCustomDiagID(
            DiagnosticsEngine::Remark,
            "Переопределен без override"
        );
    Diag.Report(Method->getLocation(), DiagID);
}

void RefactorHandler::handle_crange_for(const VarDecl *LoopVar,
                                        DiagnosticsEngine &Diag,
                                        SourceManager &SM){
    if (LoopVar->getType()->isFundamentalType())
        return;

    Rewrite.InsertText(LoopVar->getLocation(), "&");

    const unsigned DiagID = Diag.getCustomDiagID(
            DiagnosticsEngine::Remark,
            "const без &"
        );
    Diag.Report(LoopVar->getLocation(), DiagID);
}

auto NvDtorMatcher()
{
    return cxxRecordDecl(isDerivedFrom(cxxRecordDecl(
        hasMethod(cxxDestructorDecl(
            unless(isVirtual()), unless(isImplicit())
            ).bind("nonVirtualDtor")
        )
    )));
}

auto NoOverrideMatcher()
{
    return cxxMethodDecl(
        isOverride(),
        unless(hasAttr(attr::Override)),
        unless(cxxDestructorDecl())
    ).bind("missingOverride");
}

auto NoRefConstVarInRangeLoopMatcher()
{
    return cxxForRangeStmt(hasLoopVariable(
        varDecl(hasType(isConstQualified())).bind("loopVar")
    ));
}

// Конструктор принимает Rewriter для изменения кода.
ComplexConsumer::ComplexConsumer(Rewriter &Rewrite) : Handler(Rewrite) {
    // Создаем MatchFinder и добавляем матчеры.
    Finder.addMatcher(NvDtorMatcher(), &Handler);
    Finder.addMatcher(NoOverrideMatcher(), &Handler);
    Finder.addMatcher(NoRefConstVarInRangeLoopMatcher(), &Handler);
}

// Метод HandleTranslationUnit вызывается для каждого файла.
void ComplexConsumer::HandleTranslationUnit(ASTContext &Context) {
    Finder.matchAST(Context);
}


std::unique_ptr<ASTConsumer> CodeRefactorAction::CreateASTConsumer(CompilerInstance &CI,
                                                StringRef file) {
    RewriterForCodeRefactor.setSourceMgr(CI.getSourceManager(), CI.getLangOpts());
    return std::make_unique<ComplexConsumer>(
        RewriterForCodeRefactor);
}

bool CodeRefactorAction::BeginSourceFileAction( CompilerInstance &CI) {
// Инициализируем Rewriter для рефакторинга.
RewriterForCodeRefactor.setSourceMgr(CI.getSourceManager(),
                                        CI.getLangOpts());
    return true;  // Возвращаем true, чтобы продолжить обработку файла.
}

void CodeRefactorAction::EndSourceFileAction() {
    // Применяем изменения в файле.
    if (RewriterForCodeRefactor.overwriteChangedFiles()) {
        llvm::errs() << "Error applying changes to files.\n";
    }
}


int main(int argc, const char **argv) {
    // Парсер опций: Обрабатывает флаги командной строки, компиляционные базы данных.
    auto ExpectedParser = CommonOptionsParser::create(argc, argv, ToolCategory);
    if (!ExpectedParser) {
        llvm::errs() << ExpectedParser.takeError();
        return 1;
    }
    CommonOptionsParser &OptionsParser = ExpectedParser.get();
    // Создаем ClangTool
    ClangTool Tool(OptionsParser.getCompilations(), OptionsParser.getSourcePathList());
    // Запускаем RefactorAction.
    return Tool.run(newFrontendActionFactory<CodeRefactorAction>().get());
}
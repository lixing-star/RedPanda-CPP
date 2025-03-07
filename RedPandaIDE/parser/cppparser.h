/*
 * Copyright (C) 2020-2022 Roy Qu (royqh1979@gmail.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#ifndef CPPPARSER_H
#define CPPPARSER_H

#include <QMutex>
#include <QObject>
#include <QThread>
#include <QVector>
#include "statementmodel.h"
#include "cpptokenizer.h"
#include "cpppreprocessor.h"

class CppParser : public QObject
{
    Q_OBJECT

public:
    explicit CppParser(QObject *parent = nullptr);
    CppParser(const CppParser&)=delete;
    CppParser& operator=(const CppParser)=delete;

    ~CppParser();

    void addHardDefineByLine(const QString& line);
    void addProjectFile(const QString &fileName, bool needScan);
    void addIncludePath(const QString& value);
    void removeProjectFile(const QString& value);
    void addProjectIncludePath(const QString& value);
    void clearIncludePaths();
    void clearProjectIncludePaths();
    void clearProjectFiles();
    QList<PStatement> getListOfFunctions(const QString& fileName,
                             const QString& phrase,
                             int line);
    PStatement findScopeStatement(const QString& filename, int line);
    PFileIncludes findFileIncludes(const QString &filename, bool deleteIt = false);
    QString findFirstTemplateParamOf(const QString& fileName,
                                     const QString& phrase,
                                     const PStatement& currentScope);
    PStatement findFunctionAt(const QString& fileName,
                            int line);
    int findLastOperator(const QString& phrase) const;
    PStatementList findNamespace(const QString& name); // return a list of PSTATEMENTS (of the namespace)
    PStatement findStatement(const QString& fullname);
    PStatement findStatementOf(const QString& fileName,
                               const QString& phrase,
                               int line);
    PStatement findStatementOf(const QString& fileName,
                               const QString& phrase,
                               const PStatement& currentScope,
                               PStatement& parentScopeType,
                               bool force = false);
    PStatement findStatementOf(const QString& fileName,
                               const QString& phrase,
                               const PStatement& currentClass,
                               bool force = false);

    PStatement findStatementOf(const QString& fileName,
                               const QStringList& expression,
                               const PStatement& currentScope);
    PStatement findStatementOf(const QString& fileName,
                               const QStringList& expression,
                               int line);
    PStatement findAliasedStatement(const PStatement& statement);

    /**
     * @brief evaluate the expression
     * @param fileName
     * @param expression
     * @param currentScope
     * @return the statement of the evaluation result
     */
    PEvalStatement evalExpression(const QString& fileName,
                               QStringList& expression,
                               const PStatement& currentScope);
    PStatement findTypeDefinitionOf(const QString& fileName,
                                    const QString& aType,
                                    const PStatement& currentClass);
    PStatement findTypeDef(const PStatement& statement,
                          const QString& fileName);
    bool freeze();  // Freeze/Lock (stop reparse while searching)
    bool freeze(const QString& serialId);  // Freeze/Lock (stop reparse while searching)
    QStringList getClassesList();
    QStringList getFileDirectIncludes(const QString& filename);
    QSet<QString> getFileIncludes(const QString& filename);
    QSet<QString> getFileUsings(const QString& filename);

    QString getHeaderFileName(const QString& relativeTo, const QString& headerName, bool fromNext=false);// both

    void invalidateFile(const QString& fileName);
    bool isIncludeLine(const QString &line);
    bool isIncludeNextLine(const QString &line);
    bool isProjectHeaderFile(const QString& fileName);
    bool isSystemHeaderFile(const QString& fileName);
    void parseFile(const QString& fileName, bool inProject,
                   bool onlyIfNotParsed = false, bool updateView = true);
    void parseFileList(bool updateView = true);
    void parseHardDefines();
    bool parsing() const;
    void resetParser();
    void unFreeze(); // UnFree/UnLock (reparse while searching)
    QSet<QString> scannedFiles();

    bool isFileParsed(const QString& filename);

    QString prettyPrintStatement(const PStatement& statement, const QString& filename, int line = -1);

    bool enabled() const;
    void setEnabled(bool newEnabled);

    const QSet<QString> &filesToScan() const;
    void setFilesToScan(const QSet<QString> &newFilesToScan);

    void setOnGetFileStream(const GetFileStreamCallBack &newOnGetFileStream);

    int parserId() const;

    const QString &serialId() const;

    bool parseLocalHeaders() const;
    void setParseLocalHeaders(bool newParseLocalHeaders);

    bool parseGlobalHeaders() const;
    void setParseGlobalHeaders(bool newParseGlobalHeaders);

    const QSet<QString>& includePaths();
    const QSet<QString>& projectIncludePaths();

    const StatementModel &statementList() const;

    ParserLanguage language() const;
    void setLanguage(ParserLanguage newLanguage);

    const QSet<QString> &projectFiles() const;

    QList<QString> namespaces();

signals:
    void onProgress(const QString& fileName, int total, int current);
    void onBusy();
    void onStartParsing();
    void onEndParsing(int total, int updateView);
private:
    PStatement addInheritedStatement(
            const PStatement& derived,
            const PStatement& inherit,
            StatementClassScope access);

    PStatement addChildStatement(
            // support for multiple parents (only typedef struct/union use multiple parents)
            const PStatement& parent,
            const QString& fileName,
            const QString& aType, // "Type" is already in use
            const QString& command,
            const QString& args,
            const QString& noNameArgs,
            const QString& value,
            int line,
            StatementKind kind,
            const StatementScope& scope,
            const StatementClassScope& classScope,
            StatementProperties properties); // TODO: InheritanceList not supported
    PStatement addStatement(
            const PStatement& parent,
            const QString &fileName,
            const QString &aType, // "Type" is already in use
            const QString &command,
            const QString &args,
            const QString &noNameArgs,
            const QString& value,
            int line,
            StatementKind kind,
            const StatementScope& scope,
            const StatementClassScope& classScope,
            StatementProperties properties);
    PStatement addStatement(
            const PStatement& parent,
            const QString &fileName,
            const QString &aType, // "Type" is already in use
            const QString &command,
            int argStart,
            int argEnd,
            const QString& value,
            int line,
            StatementKind kind,
            const StatementScope& scope,
            const StatementClassScope& classScope,
            StatementProperties properties);
    void setInheritance(int index, const PStatement& classStatement, bool isStruct);
    bool isCurrentScope(const QString& command);
    void addSoloScopeLevel(PStatement& statement, int line, bool shouldResetBlock=false); // adds new solo level
    void removeScopeLevel(int line); // removes level

    int indexOfMatchingBrace(int startAt) {
        return mTokenizer[startAt]->matchIndex;
    }

    void internalClear();

    QStringList sortFilesByIncludeRelations(const QSet<QString> &files);

    bool checkForKeyword(KeywordType &keywordType);
    bool checkForNamespace(KeywordType keywordType);
    bool checkForPreprocessor();
//    bool checkForLambda();
    bool checkForScope(KeywordType keywordType);
    bool checkForStructs(KeywordType keywordType);
    bool checkForTypedefEnum();
    bool checkForTypedefStruct();
    bool checkForUsing(KeywordType keywordType);

    void checkAndHandleMethodOrVar(KeywordType keywordType);

    QString doFindFirstTemplateParamOf(const QString& fileName,
                                     const QString& phrase,
                                     const PStatement& currentScope);
    QString doFindTemplateParamOf(const QString& fileName,
                                     const QString& phrase,
                                     int index,
                                     const PStatement& currentScope);

    void fillListOfFunctions(const QString& fileName, int line,
                             const PStatement& statement,
                             const PStatement& scopeStatement, QStringList& list);
    QList<PStatement> getListOfFunctions(const QString& fileName, int line,
                                         const PStatement& statement,
                                         const PStatement& scopeStatement);
    PStatement findMacro(const QString& phrase, const QString& fileName);
    PStatement findMemberOfStatement(
            const QString& phrase,
            const PStatement& scopeStatement);
    QList<PStatement> findMembersOfStatement(const QString& phrase,
                                             const PStatement& scopeStatement);
    PStatement findStatementInScope(
            const QString& name,
            const QString& noNameArgs,
            StatementKind kind,
            const PStatement& scope);
    PStatement findStatementInScope(
            const QString& name,
            const PStatement& scope);
    PStatement findStatementInNamespace(
            const QString& name,
            const QString& namespaceName);

    //{Find statement starting from startScope}
    PStatement findStatementStartingFrom(const QString& fileName,
                                         const QString& phrase,
                                         const PStatement& startScope);

    /**
     * @brief evaluate the expression (starting from pos) in the scope
     * @param fileName
     * @param phraseExpression
     * @param pos
     * @param scope
     * @param previousResult the result of evalution for expression from 0 to pos-1
     * @param freeScoped if the expression left is
     * @return
     */
    PEvalStatement doEvalExpression(const QString& fileName,
                               QStringList& phraseExpression,
                               int &pos,
                               const PStatement& scope,
                               const PEvalStatement& previousResult,
                               bool freeScoped);

    PEvalStatement doEvalPointerArithmetic(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);
    PEvalStatement doEvalPointerToMembers(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);
    PEvalStatement doEvalCCast(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);
    PEvalStatement doEvalMemberAccess(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);
    PEvalStatement doEvalScopeResolution(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);
    PEvalStatement doEvalTerm(
            const QString& fileName,
            QStringList& phraseExpression,
            int &pos,
            const PStatement& scope,
            const PEvalStatement& previousResult,
            bool freeScoped);

    bool expandMacro(QStringList& phraseExpression,int &pos,
                     const PStatement& macro);

    PEvalStatement doCreateEvalNamespace(const PStatement& namespaceStatement);

    PEvalStatement doCreateEvalType(const QString& fileName,const PStatement& typeStatement);
    PEvalStatement doCreateEvalType(const QString& primitiveType);

    PEvalStatement doCreateEvalVariable(
            const QString& fileName,
            const PStatement& varStatement,
            const QString& baseTemplateParams,
            const PStatement& scope);
    PEvalStatement doCreateEvalFunction(const QString& fileName, const PStatement& funcStatement);
    PEvalStatement doCreateEvalLiteral(const QString& type);
    void  doSkipInExpression(const QStringList& expression, int&pos, const QString& startSymbol, const QString& endSymbol);

    QString findFunctionPointerName(int startIdx);
    bool isIdentifier(const QString& token) const {
        return (!token.isEmpty() && isIdentChar(token.front())
                && !token.contains('\"'));
    }

    bool isIdentifierOrPointer(const QString& term) const {
        switch(term[0].unicode()) {
        case '*':
            return true;
        case '\"':
        case '\'':
            return false;
        default:
            return isIdentChar(term[0]);
        }
    }


    bool isIntegerLiteral(const QString& token) const {
        if (token.isEmpty())
            return false;
        QChar ch = token.front();
        return (ch>='0' && ch<='9' && !token.contains(".") && !token.contains("e"));
    }
    bool isFloatLiteral(const QString& token) const {
        if (token.isEmpty())
            return false;
        QChar ch = token.front();
        return (ch>='0' && ch<='9' && (token.contains(".") || token.contains("e")));
    }
    bool isStringLiteral(const QString& token) const {
        if (token.isEmpty())
            return false;
        return (!token.startsWith('\'') && token.contains('"'));
    }

    bool isCharLiteral(const QString& token) const{
        if (token.isEmpty())
            return false;
        return (token.startsWith('\''));
    }

    bool isKeyword(const QString& token) const {
        return mCppKeywords.contains(token);
    }

    bool tokenIsIdentifier(const QString& token) const {
        //token won't be empty
        return isIdentChar(token[0]);
    }

    bool tokenIsTypeOrNonKeyword(const QString& token) const {
        return tokenIsIdentifier(token) &&
                (mCppTypeKeywords.contains(token)
                 || !mCppKeywords.contains(token)
                 || token=="const");

    }

    PStatement doParseEvalTypeInfo(
            const QString& fileName,
            const PStatement& scope,
            const QString& type,
            QString& baseType,
            PStatement& typeStatement,
            int& pointerLevel,
            QString& templateParams);

    int getBracketEnd(const QString& s, int startAt);
    StatementClassScope getClassScope(const QString& text);
    StatementClassScope getClassScope(KeywordType keywordType);
    int getCurrentBlockBeginSkip();
    int getCurrentBlockEndSkip();
    int getCurrentInlineNamespaceEndSkip();
    PStatement getCurrentScope(); // gets last item from last level
    QString getTemplateParam(const PStatement& statement, const QString& filename,
                                  const QString& phrase, int index, const PStatement& currentScope);
    int getTemplateParamStart(const QString& s, int startAt, int index);
    int getTemplateParamEnd(const QString& s, int startAt);

    void getFullNamespace(
            const QString& phrase,
            QString& sNamespace,
            QString& member);
    QString getFullStatementName(
            const QString& command,
            const PStatement& parent);
    PStatement getIncompleteClass(
            const QString& command,
            const PStatement& parentScope);
    QString getScopePrefix(const PStatement& statement);
    StatementScope  getScope();
    QString getStatementKey(const QString& sName,
                            const QString& sType,
                            const QString& sNoNameArgs);
    PStatement getTypeDef(const PStatement& statement,
                          const QString& fileName, const QString& aType);
    void handleCatchBlock();
    void handleEnum(bool isTypedef);
    void handleForBlock();
    void handleKeyword(KeywordType skipType);
    void handleLambda(int index, int endIndex);
    void handleOperatorOverloading(
            const QString& sType,
            const QString& prefix,
            int operatorTokenIndex,
            bool isStatic,
            bool isFriend);
    void handleMethod(
            StatementKind functionKind,
            const QString& sType,
            const QString& sName,
            int argStart,
            bool isStatic,
            bool isFriend);
    void handleNamespace(KeywordType skipType);
    void handleOtherTypedefs();
    void handlePreprocessor();
    void handleScope(KeywordType keywordType);
    bool handleStatement();
    void handleStructs(bool isTypedef = false);
    void handleUsing();
    void handleVar(const QString& typePrefix,bool isExtern,bool isStatic);
    void internalParse(const QString& fileName);
//    function FindMacroDefine(const Command: AnsiString): PStatement;
    void inheritClassStatement(
            const PStatement& derived,
            bool isStruct,
            const PStatement& base,
            StatementClassScope access);
    PStatement doFindStatementInScope(const QString& name,
                                      const QString& noNameArgs,
                                      StatementKind kind,
                                      const PStatement& scope);
    void internalInvalidateFile(const QString& fileName);
    void internalInvalidateFiles(const QSet<QString>& files);
    QSet<QString> calculateFilesToBeReparsed(const QString& fileName);
    int calcKeyLenForStruct(const QString& word);
//    {
//    function GetClass(const Phrase: AnsiString): AnsiString;
//    function GetMember(const Phrase: AnsiString): AnsiString;
//    function GetOperator(const Phrase: AnsiString): AnsiString;
//    function GetRemainder(const Phrase: AnsiString): AnsiString;
//    }
    void scanMethodArgs(
            const PStatement& functionStatement,
            int argStart);
    QString splitPhrase(const QString& phrase, QString& sClazz,
                QString& sOperator, QString &sMember);
    QString removeTemplateParams(const QString& phrase);

    bool splitLastMember(const QString& token, QString& lastMember, QString& remaining);

    bool isSpaceChar(const QChar& ch) const {
        return ch==' ' || ch =='\t';
    }

    bool isWordChar(const QChar& ch) const {
        return ch.isLetter()
                || ch == '_'
                || ch == '*'
                || ch == '&';
    }

    bool isIdentifier(const QChar& ch) const {
        return ch.isLetter()
                || ch == '_'
                || ch == '~'
                ;
    }

    bool isIdentChar(const QChar& ch) const {
        return ch.isLetter()
                || ch == '_';
    }

    bool isDigitChar(const QChar& ch) const {
        return (ch>='0' && ch<='9');
    }

    bool isInvalidFunctionArgsSuffixChar(const QChar& ch) const {

        // &&
        switch(ch.unicode()){
        case '.':
        case '-':
        case '+':
        case '/':
        case '%':
        case '*':
        case '|':
        case '?':
            return true;
        default:
            return false;
        }
    }

    /*'(', ';', ':', '{', '}', '#' */
    bool isSeperator(const QChar& ch) const {
        switch(ch.unicode()){
        case '(':
        case ';':
        case ':':
        case '{':
        case '}':
        case '#':
            return true;
        default:
            return false;
        }
    }

    /*';', '{', '}'*/
    bool isblockChar(const QChar& ch) const {
        switch(ch.unicode()){
        case ';':
        case '{':
        case '}':
            return true;
        default:
            return false;
        }
    }

    /* '#', ',', ';', ':', '{', '}', '!', '/', '+', '-', '<', '>' */
    bool isInvalidVarPrefixChar(const QChar& ch) const {
        switch (ch.unicode()) {
        case '#':
        case ',':
        case ';':
        case ':':
        case '{':
        case '}':
        case '!':
        case '/':
        case '+':
        case '-':
        case '<':
        case '>':
            return true;
        default:
            return false;
        }
    }

    /*'{', '}' */
    bool isBraceChar(const QChar& ch) const {
        return ch == '{' || ch =='}';
    }

    bool isLineChar(const QChar& ch) const {
        return ch=='\n' || ch=='\r';
    }

    bool isNotFuncArgs(int startIndex);

    /**
     * @brief Test if a statement is a class/struct/union/namespace/function
     * @param kind
     * @return
     */
    bool isNamedScope(StatementKind kind) const;

    /**
     * @brief Test if a statement is a class/struct/union/enum/enum class/typedef
     * @param kind
     * @return
     */
    bool isTypeStatement(StatementKind kind) const;

    void updateSerialId();

    int indexOfNextSemicolon(int index, int endIndex=-1);
    int indexOfNextPeriodOrSemicolon(int index, int endIndex=-1);
    int indexOfNextSemicolonOrLeftBrace(int index);
    int indexOfNextColon(int index);
    int indexOfNextLeftBrace(int index);
    int indexPassParenthesis(int index);
    int indexPassBraces(int index);
    int skipAssignment(int index, int endIndex);
    void skipNextSemicolon(int index);
    int moveToEndOfStatement(int index, bool checkLambda, int endIndex=-1);
//    int moveToNextAssignmentOrEndOfStatement(int index, bool checkLambda, int endIndex=-1);
    void skipParenthesis(int index);
    QString mergeArgs(int startIndex, int endIndex);
    void parseCommandTypeAndArgs(QString& command,
                                 QString& typeSuffix,
                                 QString& args);
private:
    int mParserId;
    ParserLanguage mLanguage;
    int mSerialCount;
    QString mSerialId;
    int mUniqId;
    bool mEnabled;
    int mIndex;
    bool mIsHeader;
    bool mIsSystemHeader;
    QString mCurrentFile;
//  stack list , each element is a list of one/many scopes(like intypedef struct  s1,s2;
//  It's used for store scope nesting infos
    QVector<PStatement> mCurrentScope;
    QVector<StatementClassScope> mCurrentClassScope;

    StatementClassScope mClassScope;
    StatementModel mStatementList;
    //It's used in preprocessor, so we can't use fIncludeList instead

    CppTokenizer mTokenizer;
    CppPreprocessor mPreprocessor;
    QSet<QString> mProjectFiles;
    QVector<int> mBlockBeginSkips; //list of for/catch block begin token index;
    QVector<int> mBlockEndSkips; //list of for/catch block end token index;
    QVector<int> mInlineNamespaceEndSkips; // list for inline namespace end token index;
    QSet<QString> mFilesToScan; // list of base files to scan
    int mFilesScannedCount; // count of files that have been scanned
    int mFilesToScanCount; // count of files and files included in files that have to be scanned
    bool mParseLocalHeaders;
    bool mParseGlobalHeaders;
    bool mIsProjectFile;
    int mLockCount; // lock(don't reparse) when we need to find statements in a batch
    bool mParsing;
    QHash<QString,PStatementList> mNamespaces;  // namespace and the statements in its scope
    QSet<QString> mInlineNamespaces;
#ifdef QT_DEBUG
    int mLastIndex;
#endif
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
    QRecursiveMutex mMutex;
#else
    QMutex mMutex;
#endif
    QMap<QString,KeywordType> mCppKeywords;
    QSet<QString> mCppTypeKeywords;
};
using PCppParser = std::shared_ptr<CppParser>;

class CppFileParserThread : public QThread {
    Q_OBJECT
public:
    explicit CppFileParserThread(
            PCppParser parser,
            QString fileName,
            bool inProject,
            bool onlyIfNotParsed = false,
            bool updateView = true,
            QObject *parent = nullptr);

private:
    PCppParser mParser;
    QString mFileName;
    bool mInProject;
    bool mOnlyIfNotParsed;
    bool mUpdateView;

    // QThread interface
protected:
    void run() override;
};
using PCppParserThread = std::shared_ptr<CppFileParserThread>;

class CppFileListParserThread: public QThread {
    Q_OBJECT
public:
    explicit CppFileListParserThread(
            PCppParser parser,
            bool updateView = true,
            QObject *parent = nullptr);
private:
    PCppParser mParser;
    bool mUpdateView;
    // QThread interface
protected:
    void run() override;
};

void parseFile(
    PCppParser parser,
    const QString& fileName,
    bool inProject,
    bool onlyIfNotParsed = false,
    bool updateView = true);

void parseFileList(
        PCppParser parser,
        bool updateView = true);


#endif // CPPPARSER_H

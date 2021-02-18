class mgl_scomplexer : public yyFlexLexer {
    public:
        void embedded_yylex(int& result, YYSTYPE *lvalp, mgl_scompcontext *local_context);
    private:
        int yylex();
        mgl_scompcontext *context;
        YYSTYPE semantic_value;
};

class ParserExecutionContext {
        friend mgl_scompcontext *mgl_getcontext(void *parm);

    public:
        ParserExecutionContext(mgl_scomplexer *ptlexer, FILE *ptinbuf) {
            currentLexer = ptlexer;
            currentStreambuf = new mgl_ifdstreambuf(fileno(ptinbuf), 512);
            mgl_infile = new istream(currentStreambuf);
        }
        ~ParserExecutionContext(void) {
            delete mgl_infile;
            delete currentStreambuf;
        }
        mgl_scomplexer *currentLexer;
        std::istream *mgl_infile;
    private:
        mgl_scompcontext context;
        mgl_ifdstreambuf *currentStreambuf;
};


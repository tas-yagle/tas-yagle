class bgl_bcomplexer : public yyFlexLexer {
    public:
        void embedded_yylex(int& result, YYSTYPE *lvalp, bgl_bcompcontext *local_context);
    private:
        int yylex();
        bgl_bcompcontext *context;
        YYSTYPE semantic_value;
};

class bgl_bcompparser {
        friend bgl_bcompcontext *bgl_getcontext(void *parm);

    public:
        bgl_bcompparser(bgl_bcomplexer *ptlexer, FILE *ptinbuf) {
            currentLexer = ptlexer;
            currentStreambuf = new bgl_ifdstreambuf(fileno(ptinbuf), 512);
            bgl_infile = new istream(currentStreambuf);
        }
        ~bgl_bcompparser(void) {
            delete bgl_infile;
            delete currentStreambuf;
        }
        bgl_bcomplexer *currentLexer;
        std::istream *bgl_infile;
    private:
        bgl_bcompcontext context;
        bgl_ifdstreambuf *currentStreambuf;
};


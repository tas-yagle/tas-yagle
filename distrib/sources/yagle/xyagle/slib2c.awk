#!awk -f
BEGIN {
    print "#include <stdio.h>"
    print "static char *slib_text[]={"
    first=1
    line=1
}
{
    if (first!=1) printf ",\n"
    n=split($0, r, "\"")
    printf "/* %03d */ \"",line
    for (i=1;i<=n;i++) {
        printf "%s", r[i]
        if (i<n) printf "\\\""
    }
    printf "\""
    first=0
    line++
}
END {
    printf "\n};\n"
    print "void drive_" fname "(FILE *f)\n{"
    print "  unsigned int i;\n"
    print "  for (i=0; i<sizeof(slib_text)/sizeof(*slib_text); i++)"
    print "    fprintf(f, \"%s\\n\",slib_text[i]);"
    print "}"
}

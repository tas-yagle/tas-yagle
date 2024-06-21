#ifndef _BGL_IFDSTREAM_
#define _IFDSTREAM_

#include <unistd.h>
#include <streambuf>

class bgl_ifdstreambuf: public std::streambuf {
    public:
        bgl_ifdstreambuf() : bufsize(0), buffer(0) {}

        bgl_ifdstreambuf(int fd, unsigned bufsize = 1) {
            open(fd, bufsize);
        }

        ~bgl_ifdstreambuf() {
            if (bufsize) {
                close(fd);
                delete[] buffer;
            }
        }

        void open(int xfd, unsigned xbufsize = 1) {
            fd = xfd;
            bufsize = xbufsize;
            buffer = new char[bufsize];
            setg(buffer, buffer + bufsize, buffer + bufsize);
        }
    
        int underflow() {
            if (gptr() < egptr()) return *gptr();
    
            int nread = read(fd, buffer, bufsize);
    
            if (nread <= 0) return EOF;
        
            setg(buffer, buffer, buffer + nread);
            return *gptr();
        }
    
        std::streamsize
        xsgetn(char *dest, std::streamsize n) {
            int nread = 0;
    
            while (n) {
                if (!in_avail()) {
                    if (underflow() == EOF) break;
                }
    
                int avail = in_avail();
    
                if (avail > n) avail = n;
    
                memcpy(dest + nread, gptr(), avail);
                gbump(avail);
    
                nread += avail;
                n -= avail;
            }
    
            return nread;
        }
    
    protected:
        int         fd;
        unsigned    bufsize;
        char*       buffer;
};

#endif

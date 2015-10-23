#ifndef PDP8_UTILITIES_H
#define PDP8_UTILITIES_H

#include <pdp8i.h>

namespace PDP8 {

    void LoadRim(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadRim(const std::string& filename, PDP8_State& s, size_t origin =0200);
    void LoadBin(std::istream& i ,PDP8_State& s, size_t loc =0200);
    void LoadBin(const std::string& filename, PDP8_State& s, size_t origin =0200);

    void LoadPaperTape(const std::string& filename, PDP8_State& s, size_t origin =0200);

    struct InstHistory {
        int32_t               pc;
        int32_t               ea;
        int16_t               ir;
        int16_t               opnd;
        int16_t               lac;
        int16_t               mq;
    };
    class HistoryMaker {
        static const int HIST_PC = 0x40000000;
        static const int HIST_MIN = 64;
        static const int HIST_MAX = 65536;
        std::queue<InstHistory> hst;
        int32_t hst_p = 0;                                        /* history pointer */
        int32_t hst_lnt = 0;                                      /* history length */
       public:
        void push(PDP8_State& s) {
            InstHistory n;
            n.pc =s.ma;
            n.ir = s.ir;
            n.lac = s.ac;
            n.mq = s.mq;
            n.opnd = s.mb;
            hst.emplace(n);
            if(hst.size() > HIST_MAX) hst.pop();
        }
        cpu_show_hist (FILE *st, UNIT *uptr, int32 val, void *desc)
        {
        int32 l, k, di, lnt;
        char *cptr = (char *) desc;
        t_stat r;
        t_value sim_eval;
        InstHistory *h;

        if (hst_lnt == 0)                                       /* enabled? */
            return SCPE_NOFNC;
        if (cptr) {
            lnt = (int32) get_uint (cptr, 10, hst_lnt, &r);
            if ((r != SCPE_OK) || (lnt == 0))
                return SCPE_ARG;
            }
        else lnt = hst_lnt;
        di = hst_p - lnt;                                       /* work forward */
        if (di < 0)
            di = di + hst_lnt;
        fprintf (st, "PC     L AC    MQ    ea     IR\n\n");
        for (k = 0; k < lnt; k++) {                             /* print specified */
            h = &hst[(++di) % hst_lnt];                         /* entry pointer */
            if (h->pc & HIST_PC) {                              /* instruction? */
                l = (h->lac >> 12) & 1;                         /* link */
                fprintf (st, "%05o  %o %04o  %04o  ", h->pc & ADDRMASK, l, h->lac & 07777, h->mq);
                if (h->ir < 06000)
                    fprintf (st, "%05o  ", h->ea);
                else fprintf (st, "       ");
                sim_eval = h->ir;
                if ((fprint_sym (st, h->pc & ADDRMASK, &sim_eval, &cpu_unit, SWMASK ('M'))) > 0)
                    fprintf (st, "(undefined) %04o", h->ir);
                if (h->ir < 04000)
                    fprintf (st, "  [%04o]", h->opnd);
                fputc ('\n', st);                               /* end line */
                }                                               /* end else instruction */
            }                                                   /* end for */
        return SCPE_OK;
        }
    };



    std::string dsam8(PDP8_State& s, bool comment, size_t loc);
    std::string dsam8(PDP8_State& s, bool comment=false);
    std::string dsam8(t_hist s, uint16_t* mem, bool comment=false);
    void simple_dsam8(t_hist s, uint16_t* mem,  std::string& disbuf, std::string&combuf);


}
#endif // PDP8_UTILITIES_H

/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Self-test code                                                                     */
/* Copyright(c) 2016 LoRd_MuldeR <mulder2@gmx.de>                                                 */
/*                                                                                                */
/* Permission is hereby granted, free of charge, to any person obtaining a copy of this software  */
/* and associated documentation files (the "Software"), to deal in the Software without           */
/* restriction, including without limitation the rights to use, copy, modify, merge, publish,     */
/* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the  */
/* Software is furnished to do so, subject to the following conditions:                           */
/*                                                                                                */
/* The above copyright notice and this permission notice shall be included in all copies or       */
/* substantial portions of the Software.                                                          */
/*                                                                                                */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING  */
/* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND     */
/* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,   */
/* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, */
/* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.        */
/* ---------------------------------------------------------------------------------------------- */

#ifndef MHASH_CLI_SELFTEST_INCLUDED
#define MHASH_CLI_SELFTEST_INCLUDED

/*Include*/
#include "mhash_384.h"

/*CRT includes*/
#include <stdlib.h>
#include <stdio.h>

/*C++ support*/
#ifdef __cplusplus
using namespace mhash_384;
using namespace mhash_384::internals;
#endif

/*Abort helper macro*/
#ifndef NDEBUG
#define ABORT_TEST(X) abort()
#else
#define ABORT_TEST(X) exit((X))
#endif

/*Assert macro*/
#define MY_ASSERT(EXP, MSG) do \
{ \
	if(!(EXP)) \
	{ \
		fprintf(stderr, "FAILURE: %s!\n\n", (MSG)); \
		for(;;) ABORT_TEST(1); \
	} \
} \
while(0)

/*Test vectors and reference outputs*/
#define TEST_COUNT 10
static const char *const TEST_VECTOR[TEST_COUNT] =
{
	"",
	"abc",
	"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq",
	"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu",
	"Franz jagt im komplett verwahrlosten Taxi quer durch Bayern",
	"Frank jagt im komplett verwahrlosten Taxi quer durch Bayern",
	"Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.\nUt wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.\nNam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis.\nAt vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, At accusam aliquyam diam diam dolore dolores duo eirmod eos erat, et nonumy sed tempor et et invidunt justo labore Stet clita ea et gubergren, kasd magna no rebum. sanctus sea sed takimata ut vero voluptua. est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat.\nConsetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus.\nLorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet. Lorem ipsum dolor sit amet, consetetur sadipscing elitr, sed diam nonumy eirmod tempor invidunt ut labore et dolore magna aliquyam erat, sed diam voluptua. At vero eos et accusam et justo duo dolores et ea rebum. Stet clita kasd gubergren, no sea takimata sanctus est Lorem ipsum dolor sit amet.\nDuis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat.\nUt wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo consequat. Duis autem vel eum iriure dolor in hendrerit in vulputate velit esse molestie consequat, vel illum dolore eu feugiat nulla facilisis at vero eros et accumsan et iusto odio dignissim qui blandit praesent luptatum zzril delenit augue duis dolore te feugait nulla facilisi.\nNam liber tempor cum soluta nobis eleifend option congue nihil imperdiet doming id quod mazim placerat facer possim assum. Lorem ipsum dolor sit amet, consectetuer adipiscing elit, sed diam nonummy nibh euismod tincidunt ut laoreet dolore magna aliquam erat volutpat. Ut wisi enim ad minim veniam, quis nostrud exerci tation ullamcorper suscipit lobortis nisl ut aliquip ex ea commodo.",
	"4nfPXx\"DhcA7*'\\1;2DMhbL5f%?Yk[2]P=F*RNX*#rFo>+M,h{afxB<m_$=s4&.9~)PSYR^bs&A6JA^'>II/kH''\\%3@4ty{Ji4F|:hOU53mM]l#'^Z1rKoLU2@^g2B2MOZz:R+v5:Cp8eaRe~Z>V&S8,{]akDMsReuuarYB)_uPW]DY+f.!B=V_$H=6TavZk.p.RRF?~zdGoYJncq`+-`ojTmq:669dsEfE0wy|RS[^SzImF(cE}Ox0wLJNjw`Td,)zYgk}v_'0!fJqvV-tWsU]KP6b]Otha)0/o'QYfaIZ2I{#!Z4\"<8s#4\"M$|#;';+Oj}sPTQDNo4ILfVpNC@QIgp.W]s&BjlXm=u`:,\\\":RTh<Q!exDec>aB+`gEl=+:a.BAv8Sf5f?'s]hK&J+]\"u5@5`Lo_Y-\"=|;rAT>FR4\"D=c_aD/?Y\"HaGU^nK[O35cNo=C!y^_1\"AF8YP8,:dYjes1.!!'sNiy?e8Fj2//(vWO>N[b'u_-8pnX(d>^`^uqp9\"}m@#ejYbV**SJ]>|2m'lj,Z~#SxU%p$J1_B!r_f{wTlBW0RLS1Y:&G[l|\\_:amo&LQRJVix|@(^^6`w(ym\"#g,(f8bM=l5t0;m.aH0Mp,S9SdCw\"',*?UQk#g^L1_3:>g+Q6~'L.~2b!Qq.qa6vdMyH&JU9\\b5=QC=E\"EsBssy}G~t#R^pyKyR163bZ@G'|}/5,|#@)c!!Qm2e<ETW#=B3n9fW1B5PlX:bdC/oB}SmF[iXc;gi;1cK$eR%$|9APOTVfISe[*w*4?,k(ktt,Kd56YyY}(2z5e}Y/|FGh_Y3aH)m(e.WYmI~u9YR\"1Hs5v/j-b`T.&S#o)yiglxvhF2d]E<94;(r+BY#g::pP?Gl<di\"z\\jVX}ILca}sW9;O{iH<9kpMO']LR6%\"Jnj}Ys--,bO}$7/sO3i@knb>\"w_m7FT]O9O.qg'$7j.v<|f@p3`r!Kh\\xbZSN[i{2/?X>EF\\R&pqi&>wW!YvjQ)lD9Xt{_Y}TVPRGd>x-Po8,4nfPXx\"DhcA7*'\\1;2DMhbL5f%?Yk[2]P=F*RNX*#rFo>+M,h{afxB<m_$=s4&.9~)PSYR^bs&A6JA^'>II/kH''\\%3@4ty{Ji4F|:hOU53mM]l#'^Z1rKoLU2@^g2B2MOZz:R+v5:Cp8eaRe~Z>V&S8,{]akDMsReuuarYB)_uPW]DY+f.!B=V_$H=6TavZk.p.RRF?~zdGoYJncq`+-`ojTmq:669dsEfE0wy|RS[^SzImF(cE}Ox0wLJNjw`Td,)zYgk}v_'0!fJqvV-tWsU]KP6b]Otha)0/o'QYfaIZ2I{#!Z4\"<8s#4\"M$|#;';+Oj}sPTQDNo4ILfVpNC@QIgp.W]s&BjlXm=u`:,\\\":RTh<Q!exDec>aB+`gEl=+:a.BAv8Sf5f?'s]hK&J+]\"u5@5`Lo_Y-\"=|;rAT>FR4\"D=c_aD/?Y\"HaGU^nK[O35cNo=C!y^_1\"AF8YP8,:dYjes1.!!'sNiy?e8Fj2//(vWO>N[b'u_-8pnX(d>^`^uqp9\"}m@#ejYbV**SJ]>|2m'lj,Z~#SxU%p$J1_B!r_f{wTlBW0RLS1Y:&G[l|\\_:amo&LQRJVix|@(^^6`w(ym\"#g,(f8bM=l5t0;m.aH0Mp,S9SdCw\"',*?UQk#g^L1_3:>g+Q6~'L.~2b!Qq.qa6vdMyH&JU9\\b5=QC=E\"EsBssy}G~t#R^pyKyR163bZ@G'|}/5,|#@)c!!Qm2e<ETW#=B3n9fW1B5PlX:bdC/oB}SmF[iXc;gi;1cK$eR%$|9APOTVfISe[*w*4?,k(ktt,Kd56YyY}(2z5e}Y/|FGh_Y3aH)m(e.WYmI~u9YR\"1Hs5v/j-b`T.&S#o)yiglxvhF2d]E<94;(r+BY#g::pP?Gl<di\"z\\jVX}ILca}sW9;O{iH<9kpMO']LR6%\"Jnj}Ys--,bO}$7/sO3i@knb>\"w_m7FT]O9O.qg'$7j.v<|f@p3`r!Kh\\xbZSN[i{2/?X>EF\\R&pqi&>wW!YvjQ)lD9Xt{_Y}TVPRGd>x-Po8,",
	"xH-5Pm^ISCqug#4f\"LM6uv/K848USL$H$G>YlzPvSE5(aa&'l@'?oKy`H;jicu=0U!OG#DLI;Ep[.Ev^h:J;Qk1@5pPqfN:q#\\K0p0]Y1lU[CP/5F8={tgMG_|`)314)9]B~Wb&xb=aBHWm(*RP%f@w]?$B|}JTw#{K$Z|$_k=rUl`1Tgvu[Q,6o([y+NJ?VWo7Ge;3s6S*Se#rF3QjPYIU7.S2d\\J!hd7.<zfUdqPmIgbBU_OK1E_-+c*!v.02>|XHsqHJ\"@[g^h*\"8<sZ?sZ_$IC'_[bIuqx0ndk#8,tJ\"jcX>kEjwfyj|\\/..D\"!OY4\\~\\zzZU^ai&'Tf>`ohw2~RH@70Nv(^g)L-K*F'N|?LZYR$-{Fz7]-<[_AS33bsgz{iV@!(B-UTq]w4+M12G1=fsSrJdXJisS)~kU|J8Za,pu*fN.EBY6*|Sr@BXlVU@cb@VEGJ>tnBx|}Nq'=*W'-qJmj$*@!qc^[t-s!<)MgKy?N|~REsh^3I\"jf:E~&xpe6h=aGq>-tEtQ/xV`)FJH?Raa.\"Yt8k'{7S8\"\"NuTR`}iqee5d3ntWj\\)gq$b$[dUOa,T4*3Zlum+l|fb-\\ZBM5U/H!St$QpgwK]lO/Bo]\\!&2#uAI(IKQ`EFsGd~i\"N>wm&K+$TSYh^x>gF!&^B+\",{[unp.>APFH3*dh_%/0Gp-Jqdh=`OFtxwx1zLaZ}?xErfrhd?@X|CEn[NAf=M;N_d'YCn3B/`<c?<|H{h1cJo/0(\"E:aG*7O4t^7IC|'Z0QD-]$pi;b3]+zUq$wk(Tt$^s3!p%4x~CnGkiq6ceDU,7{/X`kv|[J6]pqlis'N\"@SJvcp7Cah)*}`>9$BV-S&-O?mpa5I0,O&FVV|%<mt&,qw\">u5I*JY>gV;$G8QD$HLE_l;O]@tb2)XP$TT?!n6u.rZILDsrc<HW0KO@,!zHg}Yah7HI@;RNsqiToEdUj>Go4IoV>pPN\"X)W(b@Bjm.PEi\"T5TN<dee5{I;m<tI'%+xXx@*f\\m*doIM%};,ye49-@Z#C2]c3BFacDj.Bg&/3CoiJoR$s\\{Oj#Ja:.xL2faN[c(RF)Bfn+6yAzOpBj'tJ/?>rEc'Goo_LYEtJH-r.'K9i,1VmdUvUT{FPc{jOGA~w\"wq\"n9!+sO8x6KXT,fD&AoFFhmH]h~ax~%-J)h9AbA&CYam|,IL|[\\bS*f*)8kO]5:]42:m09FT5d'Gw]l)WA:!r0\\WNmTF>LJdwl&{:uyjswtADS*^s(PvQ4u8!*XDow\\'Q&bT;-p.9@<<2toyiNE\\/A+R>g4u`qyc-^>2:A]F;fA(&55VQR@5f-mzvB^-\\uny|W^i|IstzFr%pR%qTh}'Ee$R0nq0t2TUQp]w`leqN}J)])SGKDSu)_,gF`\"#RmG~NmA0lCAE/Kj0buRr$}CZZ0^gF|dzBv;PKAyAF&sk_S2'~\"b=0\"f!-`J;`fvo+k$3HvU*&hIL*A1D4nc#h0g'I]\"eI1J]\"dffZlcgu+%H'hC}5fOY:*nYbty`c6^>(V?::X}\\[3)9HE71y`mKF@c\\1LD<<|,s^+\\gf*\"n~3lb4;u>~Kn[#:uY0zjTDG=3*m9*AX3f/7]f;nT:f1z1ghXB#ZT*Rwt[v\\l=w/\\A4w#qD\"~}iF;tWi%C5Z|vC8pqv\\[VM8,!&ZC'@6_tINR2TLE[2MSGyuAJs=R'(M@1K+V\"pSw\"F=Gpzom}S2xqbl\\.sMv$*K&60W2&g^ANR_Xs~ssUx1\\y\"%L+U/)v's6BR~aGC!F,;)(S-8eTeA(}D,4!T0H\"Ip*tQYIUXUM$iJ?@(gS][n%FY24u0G:Hh7>49+~]1~l%EWHiC3\\c`x~C;P9&g?=o6OKccHIC2&?bs>\"6{0Uk$D};X~l{cOzPx)uy0SmU<`K;&romGo%av<c'-^W|{p^}4\"t+K)iG>#1uMnvY?^Ato/o1*/+(}wm:Vimc0@M-x*X#-L+ZrI&7DSl(oMqDzk7[nht\"JIu:eiubYipjD;u0t`Gz/Y^1+>?L)uDlP2ggskl,i8aaMr,'.DwR@[O@!7o`&=Rh;yA-.g9I$MNZjZ0VR\"[byEPHu82V26xigkPU{1|1;cE[Nc@w2-p+372dZ=y)U[=au':P[efVUWO+y1y4u2|#\"|R],M<l$mK0~6|<p|G8o-46[byoNMG^W%\\'+F>jvK?4g'V!V[eS#Z[`shmE5o\"j?nWMF|[[ux#P0,?O!L,oi*1d}ik5>$mGt#=</W,7~,BMVgfUbpwb-{9]:SZNLaf-zAf\"5gxtx:_R&+Ej{EdiurnI%Z#A\"qc5PK\"\"x^\\yc*J]TL&EE9z>Xwv}z>YOh]l9G[ZVl;<^Ik^8zr;*$dn$@LT}@|}=t5<3o3'^dC7dFk<f)~|ew1REb:?wUyce=2G:OGuAY-f&>wnU(}r{IA;^Tj8<odqlw_ll<;ky6L27e!H4Ig{a$7ksy{3=?AQ6(K;:*Ax-+2@#Qh[59%0LeM_'^f99*/IeqB{|LE}|P#^UFb<,?TBS$i*ghB6xnddpc/l<@6nF?CDP1<$`=;v|(@vYJ}\\Z1iB<eL/\"QaKZ&zv46pp*H#H&JC7icPB2s@5j0CqWtDCWSo7~gzl?lO=]ovB7gW`dRCc&WW@Z.Jwkyzh\\wY^Eq,@=@|0kgm)iD*x|2^EJdhTL#|eU$\\n(DpeWB[sjOVqNh@!eX:HqJcGoH+;rw6Syp{]fTUm>1Gaz:1'I6WRYN?OTl`;IDX\\D{^#3(!&744;-Ms,:BIouf'PP3@Ec<[9&1#JRceAc_9?:CiHxe63=~\"d&S)SovS75S+s\\p6JkFG}LSmK=f?tdcodC-!%d*tt`iXm'WC!fT>5)99_OUOUAByMpGMbMp<^?g+3p(_y_-tfNod.BC@E4%N0{_Yg-m,PB=_|ID4j<HcKs=>&d\"KC+fe}I03Q<,xbeu$zPL%|`q!}4C(xH+l08*<dz}Zy7<$Dg3<=peH!E{:$QQj}]Ldx(OcVuh:n2:RLPU\\i\\6:~*3X#kyEd1d^&GpX\\.mF2PR*-x;{t|YdTc*\\'#hhG8e.8qTj#wnLr#o/rK&so8,-|3[t6Ws_&riO_n6%&t9-|3HbheYP_=zkV{`VI\"/>laQ\\m^5EfGv&>0'oa}?!W(8W-e.w<Gtq1>^`_|5%^.d~hi!rx'@dmdLicn[M=\"e_s|+p=]S4d)x^M(beqVc4b/Rs@z9+~aJ\\e/oO>g+Ls+\\^/K((:'mMzuBG~|}s|WzKf.U/Z3NJLr6KD*/k7cTut`ru1],quA_c:2ezpj>i8b*6ch*,*oV3[gEs2:|2t1C7$L,+6N$t`KVv+~{cjt=8XvNCBM{,V3Bo!InB\\ONl-,~.Up}$.H(u0:!v)X~hFqI@kOZA6}Ni674G`[LlB'&\\TQX`b@&J82K093'X=][S1>ot+B(!cJG8Y~&:?B+,KsjXdAGm\\Q=*sy,U/SAwmOU`#ERSnuXJavc;|8m3]1wA2):,]K4y4ERzd)R8+NY\\rq-%S8a=0J%U#%cV::8\\a\\oT}99#9(f}}nIhvpa6+Zn>0E&+MhL:dI[x|bzIPwwv>xHZf[%I4L~v2w\"lL-@~cJRqTVhdTO00'#APkQY(Zv1/}\"qN>1b^]=!00}r:Ou%NlMD!3Y_C%AtvGs!fhR~=yrFVJ$eQVjPU\\G1r&j#1522O+LX,N2}q`?PnIRA8y;b{RKT0.-,Nu/5M~uAP~Gl92Qn<9hb`@>.-\"2CL)s7HOo}6@kQ+GY{e+\"UnhJO|Irzp@^\"x+PvRzVai>!~1sz*^`%!ay)|kB)u[Ls;@u}(W*-{gY<ARRq?X:yTX0OfO.dlb{$$94a}1^R&]WO)bAXXZ;ksL@so6T,c+WRlW'{'G8Gn[.Q}H!r<N^%wd,EhH\"dw^&UpP^AIk3Ixg/;vJSiAcUVFk|]-d5V|hjZB@BPs#F[<dpBMLa,V/nXa@O=q?/(tyALK5S!Ih\\\\w4HjHEnW-ednl`c9Xi(wC~%Rp0i_`z#=%_D4FIvk;C;MuGmj/xir\\d-.]yy\\D^!X03g}%7n_9X452YJWN<)76/S~1[s0u+%Ao5&t+q59j379Yp+y:+=/w/x2b`@#GoFSf'o:=vT&i1m\\j%j[OwVz5Dw,H1J%P,7Vtl]~8]b`~cayO4R\\\\n#S9",
	"XJ}Zpnt1xNNhsl+\"3[9K9bdQnq|v{vk1_,~!L'*Lih=`{o$'y3S}:F\"2MJR\"M,Bw\\=VpoZ$lx+6#DJ[/eURe4=L8;HTpK%sPDf<[7;.K2t@G3z>OCZ0v=V{$;`]K!.(EYCN@dzRX,lr]HygmfFtU;zBL5z;)/Q6{KMpjz5M?A;c$k3Oxc.i!yHF>}_t4/RUc?)HKLv/Y=jZpYJ9Z!Nj53g(ePk$~4)a3|!geP?.Ipy3=\\\"iUw$N_HmXP_yEK8S@{Yq))i=~:>^,K4N5%D<'(Q:0uf&VwqaxDR1%/}n1>~,rHo\"moQmh`y>&Xf_b/w);BmIpM)9_WBxn4w$;-8hiRt%\\3/[q&<^vMq\"2bQSOmqH|o7SjX~?a/Plnn(8Qs7;%E7IR0U^s};njmmlW5f.i(1gr;#9j0f9F^~t<S8MR2&0uo<|#NJe-zmiYOh=cbtD1_<UJW?&w[%hwHoJy$M+z3?RjL55^'M/zYkO5s23&p|n2f^'QbMs!1F*d$8IOV7tPP63Xd}CGHN,hbIwf2Pw.I&*MZ/'}E{^&tS[ouat,%\\R*62_y=,K|xVA^^[=?nw1!xR7D2*N[bMqH0#ZEc5\\zzpKELCE`z[be)$1,EegU0WC|c]4fZ+C3!5l.'?:`/UMv(avO:]Q0v&}4:?PF`z4B1~o*2{HPtx|2%[LS*c<uBVTlHjhFU#&<q=Z:J7XSD&VZASUD+lg*Y|2:xb6.~-cn4A&Yl8mX-9M}G@oP?O<VesD5AKW6*GQ=.C%`03bj}Nv0-Rc)GYyaShuBJ7Rn8`S}Tg5IKTuhjxwN_o/nVC(=VHEDKQ.M#;45l.sSrChuQNtey,{V4$0Fxg2%&iO9C.P3bt@19}*2|EPIysi5yt9|3-@C`b@LmK,!#')8>s+6+`VZOrFtKsCLInbH{QFEgJ.BLXnMhqO@OQGeww#B=]+MS=qmnj~RXx0a4MtgdQ\"7A7^TtguH=,-Nrb$r'`)G_.Xep\\%~{ep;M408\\>MRdp:[C>}?)H1H'#;jA:++z]jiV,)cA.MHb[ZFSK?ROteR!z6Jvs1b0/pZ#y{ByYxAyg]Z*)o#7I=bh8`Jz?LE\"/-h7rlu)<o'9u&c-x3zDA=yltl~X5GsjxF#S)TdnyqmQVZqpLPFHU0\\Tm137~|`V\"7Qh*<d8mRCG$O^M(``R,wN=A9o[>.rC<LNKgqKorRT`ST~{\\5.2|]qj<|H@D@:|cCfXSft/_](*>xnJwp54(vz,]~_*=3e:Zr^&\\<]x?>)XC<g6?5|HE}vb-g>=f;6Vn.VDlDfMNb1pE#60={}].`?klW}\"Tu(v)U0nPqh[#U,;C&\"&iB%G%7PM9*JF3h?zXD?Yyrs_G36DJQ5Lbu[R/qYQ]8>[ePf{v-~.Sh_15.MU48:eoX<Py~$)^JQ)b;q+qTEr@57f`[xGoJZBh;Gub-<>qU>+7,G<RY-*2r3S%z)0_8!(Q1TdX%A}q_oR7e+U'|U,zQo2bi(HHX#?!FF%}LsgGK\"iX+JM:nT6jc5g0fvs3FWvP/+H/@-MoZhK\"nXoJ|mU)&Wv$1$ALW_~vI;aQm~e\\\\}yp``O!xele#OF=*@=J89g<8h_GS>DXcyg9.{*Uqt.3P34rFS'@.*z}T^KW>WkA?\"J_BOk`;zX\\w88y1o:s~~0ldTf[Mx^*>^DG4T9|:B3#q8&)IgjwpVc[_sSKxeO#0]gaA4PQO@*)'>C||ziIh|$hY@{:g3~<.$p0|S$$%5LJcvS4xURN`v~\"5$hLqRzC]wRJ4|yqB<x<1Z[U!2pst7)v^|\\9,_CS06+|LGe7FOjT$vOHOO&?n-UD'ecuS|]6S^>3D-0qJ_q{?\\9^?DF|S+n#5e6~xVY)p4?<R\\<>IS/<#ln5d{F:zf8S}][b7[tbX$RN'c+xppU`vPuN:?LPD_$dnrwsB&sEH)[-+<5D}mqb`MGLMQ&~aEqGgAMklbTt#f}r:*qkf_cE1ptxTZ?/m&\\J[QN\"30vu2IY9^$kR\"a</r'Z]*v=-'4~Kr1XSM%E<m_h|0MS!\";8@HpzMHzNC^W3}$GY)I;Zf]`A,PM)tnfZM06&M)ywHCBW_F6uan._M)'.2o^\\,/OlNgXvanc(4T)e;_b]d{O6@0xHm)f\\9RbT.hLh#-7:ty.ROvVI&sT0%qiil+qh:VD#~ElW%9lY:kz3{CFxu2E:p^`Hl2^xOB,geo%oHJ4f#vFfuuNpw/l06>Vm/:KJ'ZMWNBQ_ZgRoXJ7+\"z\"0%SLCP%<m}Qm[$<I-&oo:%hW;emi\"8/Y~0@g!^H~(GK{a~>C'9]i)Q~Vna!CsG#dN$Q_Q]Nqx{)8pz0xHH^JcY|irqpk/\".b=q1EI2*hEGY)93\"3.zEBl36&[{5Nl4+w&3EG'in\"ti@[W:*8E7D!z{|D`?|Um--1TV7CCoOP5AB#O=GD^Kl^XenqL:HI_eULs#ee)ssNN>sQ!Gv]@es0@IWwA'M}%XJ!p_g$RCj\\N\\T99EHiM~6`Ggx]K~noCP5*\\BMUP-7C@.Eqe7H8~leX94f}m;Cd@5-zSWe76426}s*QN-fmN(vG$)]bA7c]-^>P%+<fe[];914Xmvxu22a%:nPM6`(djF6:A`b+O&%t,hE%ZXg~#^dWh!MyPYbYvZjBgETs5IC-^3]wDw00PhHOc%%_l17!s6ky>'i~u_)V!M@tV[Fs7{Ls-b?|*vi;^__f8q(yVEd>3FxrOTwep/*<N|_KIS%GXIw*dtyUj<^(Q3N:SUy)ftu5v}?=Tmcnt_e-MkAMwwK(er_gG@WJoWCJg<6<om%r(~WvsQK4O[#n#(CGAxKfi'buWalivC!`85M2|\\88|4T/6,j$`/`-L@y\"yf~\\LYXU5a2)xf10;$S*JTaL4PubiG$Iv<sq^EzSI`;2FcGx~VlRV\"}N&#C|6f<B\"{8H@NbV=.Aox|t#)sB~$(I$r'j6B9`JCP[wr!%~{;Yn@\\U_#XjlBw^KkCv,.;o?5Gfo[7556-:[g2qV_>3]ao.<9lW-A!3FK?lF&@Z+_S(AxuHn6[,2TZf8rT?8#8)kInq#h@'z]~<`CP4B2{hh`+&5r5k3Y9(eC\\X0#lJ1d32R^BrQxqxlni+_H74D_|oU^Gr}pt>S_/)7__.pn0y&I-[S*Dc#(kZET1vzHGJ/T7p?:_vhl=}XK1XRyXw0$uM,6k`6R~d+--,p56~GG\\+VviT<^Uz':0B*5{P6^m<ZX=/,TRS_{dNLo.G.Wcr8a.y;-[9tW|ZP?&HbflZX\\^TyDGjk?9n}[sgn+xme`p#|G:fsaD3o[BQwWDeEDJ3Ga6?`Qh&|,J(0aY$;]QSl8\\a|lcD0wp*L?5'q}(,+SiQBWt3[[J<VA^S#_?4izlrJ68st|vGOuu8.c).=w7]/Jxdv`a7UREDEE9)x9`%;<WU,8pep#k_1[hiF=gpZ,jsb;;Pky0ZPYvf[24$+0sff|k7sratO?;X-A0Y][oRz\\rCL^_B7sIT3BL9O~Z\\t0C\"d\\FU7Q(V_0T]#k.+^SiNY95uho>{rc{-Pu\"%<$!`Ibg/w;]j$Mi<q.Lk$$Ep17zi0%mZD9X\\dWe{;oh,f[nf<0mCe>B<1,$'>1E/opGsF,fa-`/e_ZE\\U*94v[qJ.0p46=#$6QHZavH!f^t,K)Al4g5=,PbY'u4$K:mJ\"(i9Cpw$0TiLM=cVNO+()Yp`zGZ4'8H|+^R:)hXju/[G\"qfo=H*,Ep8Jc?zP0U-,%M2+-lGR/s:|eV*%C~v(?257CTDR0quCOL6g081H)O$$bqwF4^n\\e1nFvPNw:/0oC\\tu&.D+LX~N*LtPPAr0gc:QF+W2/X;q_#P20HS!HpJu:#sY~.>$t;siwyr^[3iPeo[}$!4tciN.<3,l6TGZ>Yc0DyoU<mIwW6M.WEAd|$;u+sr,_;ievqi`D-_U04^d3o.!?imF^*/U/GU?6*4\\>|6Sxi(,nAc,][@WPx:vP%Vi0'57TVHCnmg`x~Ib5^Y~_G#=Aw$[.ydp6,z\\R\"fOeCV~&&1(K2(gNXj]^[=3Pq{7<~?]>J$o8(?Ox8ZsT:p_5.@L(hBNVV<Vj?{+K\\bBY*t0>c*)1z/K*<'CE3)k}Ys'S$Tj?Wvb.fb+y|\\YSz.30DS*jJcWM7LB7&6y@^{(sCk+yYkVXfZ?;I~5$$&<-n:n*X:$:K==q7i`(W(,Jl=p.$qb2GlX-M5zX!=TUklG,\\q8-XQZ)eX#!PywvOIh'MP>^pZk3Qp89_\\]jkFIWg:sr=5@t]yzkp(Zy;!t!Ywv`j)nJOG]a?at-(-|yJ_2BRYc[L-,69?]7VBldT84<jUq+vT`dqcnHvWdmQXY/)Odg6;:Nd].R<&=@dAcrZR&|E}:~x'#n3<FRvYSh'#:%.1b:5Gv,(9#U3rZz1>22Je?8Y1D;0hN-av]ukt#Z*jti)\\x5\\j\\wVMx?eiNOY)WZ7Nd%;J~g~\"\\fI9R_bcDmh&b)JJhm%7A/,AExs><t*CkYo#*#6V$vjVox(>%|/17s.gx+zPwq1Fj|\\IO|<0=402Hk'^w>L=lj9bBZxlemVKL]8#/{T\\Y%kijgV\"Z:,EufM$4vmBo\\BXNWMs24E?,\"H\\xoJ@2f'/9b#T*@Vb7`G8oB~xh5cFtTogxNEKwg1'@!ZkP6A=XgG,u3,ug`;Vs@tT3L@q#u`_NQh5OSxKy]:*:ME%F@oY8M9N>}5Wq4'F2}'Ad]4?vECQ~kjiEwzx9<S*jAL~uv/anO]#A;<]HkxJtvgjK>Noa:/vANqg.@(2p[T}Fnm)FJ|w*/pr|Pep&2[|QQr?I11RU!;8=H/+l`IVd;(\\W_hf1+-UM=\\Jv+mA!aka[bNMP05UO|hJh=5_*ynen=\"e/jL+?D$D$\"cqa>Z04325OmYY#fsN^`|wM/mJ&\"nkT;\"0*2}Y|Aj>A*-VP$m~c,572u%IQWkdD)PBz'h|};h(@c\"g(A1,'IrpM=j&=XLKrzj\\3U{OpPr0i,^8/D9f>pw/vST,v[8nt2Dj6r/HQp8i%9a}\"PHM9).8A;}tru3\"rgZG{TviH6J:^1bCm,Qs\\_%Z6`5_}9N$!Fs(}R#6_`Kp1qE\\!fRbC%_YcM>8Z_u/)QG]:fHH8\\?v[k>uM:B=Pr%Fw'+2`D]A2S\\]s^0VkVRE!r{U$Cw@63)KEPi@OjMoOs7*R$,mUCF5w#HY#L]7TolPqZ+_G,vj[j&EfLroszQ=;xf%*imysb36M0hu219*&Uf\"z6nmgZN<'dy^Z+O!.)nXVjVt$90eWX7{rk\"Gssv{Osc?wwi3`{CJL,Rj`IvI/r2GOPoJnP`yr([s@<mn$&WPyWQrJb>Phw`<b`)v$sUdy-}$a8\"j!{_]Xz>9ijL6^C9fg[aG,K>704PCozH-2u)0'|>T-e*?_0oy@vdd,sk88XzuK*T0gu#%\\\\;dU2hlv5hJ~M{NMqH*<lw_V,UE[z=\"($i,KTPhWn0'.tf09C$%?JXhoHDrH#[~c\\zwZLrUezwGaidk/lS7N,{d=|E=?^,DgYx^F;>~yaX31:8v4]bOzC#A2bC<j_T|nf8u*1*2nLM6FS/%]3EkEcp@n]({sW':O)fe61G\";'PodS[)F7vO:TXvSg2e}W2]ompFoMJve<Y1>\\GT/Lg\\QwP]x*;c\\t)FGlk.`*&#+q\\C3{T~X<X0t&iWqWhezEN0_#CH,&V7ESf@\"=-1(*1DEK!DXVM]pQn~]#8<8#cH9ue,f4;|;8djB0`R+Q5/|EqX-hG|M2wBRC@|Xu[D!q^YrD6'7X-vMqc).Ti!6]mCof^Ez+T,G~aCh-J6Z~*rn0{ErGXvw?:jo.#%6XvfVEW#`-UXw9N0%t15=wK:sr3ogcq>XVO/8E3kXW3T7EU_Nq$qJ[i'r;y381:;WF`1\\E^6?5up#I`Jae{DOTe;!v'z,rB\"O87w0p>ztTCdKUF!+twJ&G^:6-$^`cE(h1mm2f=>O\\Ei1HGa7RJ@v,g>7',#*,1Lr!JK{&{0*}jz506;O:j)NS>EqmCXWO.(])bR!RG`}Om:!N9vym1?LIh|%}i{yh<R=$NW\\8v'\\g_7F&48ZW!r=nwc?2pN!%/?#zf2/4)i1pR!2Bf$;p>Dvc.\\~\\-IB-e~NAI7puTv<i?zjWiS0.]Cy&fbnz+ryp\"ZR,R|h!Vkn\\u8.,FolaErIi;DJr\\cv)5HHT>%B]7@uw_a_oV<,C&9p4$6x)B)Ry+/WOx?F]jeZYx-/!si02iG-*2Ry-K_=|KT]iph232\"D'*h<kW*Q-&fFOcW~Q/0LX6ej#f|(-Elf}pd6_>\\?)-]2GtVcIlE..W.$U?aNT6*R:?&qoHpS~4J.DefMz>oN>k#-^st/Z#a*46oV\\/\"XdQ7^a8TTSn31;f?SDfG]`$joE9o;/4+m_2,$HB5NO@qxg/c(B?(a)>0V2b(N$Vm>WYK(}#<T_,W\\fV`W-(&w{`}];vs<_6bKp/^P8~#R'1yo*6Pu79Es(4Rb6hcf=nodmZKwcXjw|T_^C3/CC;kL1{XRy\"|!^eo`S$qoDe\"raPjR5,>(Bi\\5xSR$gh][ck-a5r7^`(_Rh>h`WCA3zYgV+,.]kG!#R~@Mzh+c^^<\"BleMT)%E%%I/DMLkDB^kc\"X_LhXci\"J\\Sp4kl>ATGrl/|:bR1YDK\\uMfT.8*nO#Fn(L)a30zR=A$(p[sqr`J:n}lm>=?\"O-k{AG5$q,%-\"L<C=3(Z(.@}yg;*dVI3urMow#m1a$s&FQ:N.~m+5O\\aqh-GuWaK\\Br.Kh=|CUJ<O)J@S$}U+Y}EQ6t>t[fh~'X1fgd&\\s5eg)aG>sn^9o||$dBu{/EWFWTe[q1%}rD{w-|fb\\Op+f)WJ~^eY'Rpa(rpweHY(P&$vM\\m85MNX1^fXgV5*Lp|5vMC%EtTbDw|[w7>%)WT8fn_8R3'&f5UlQ/CfhSgP.%mZy5c3y.+:4n)TRa*hEAAm/*S#-W$FA#5k4<Q*U@miDS>c35=!A\\40RrKZ!DL)UCH_}@Js[cv~@g3W*mF~8aFKx6#`%Gkpy:}.9/G$[~c|<#K^h*X9iOnf9LZ>2?T\"Yu+8o502BtY[pFMyh(XDw@}?vP~tYRWB:c,JJ)c6kt>DfKOMR&t'ZC$#n(q;t(sE[mMk.nC>G\\Z4\"JrXd7i@1-Vhn5JVt7#4j2v<T6Uk+sm&Lui_mw:o8Gxz)d5='HkplV+Mp!FK,<Ju#m?gwi{b8L1r-H5_\"9Vc8pYvnYtQo=7-d\\4FV^,_[[WxaBV(9x.wEsB$;91L;ITij&)R!K!O)>Q0f&QxkJ`v&ouWX1IEc?kzbC.{%got+%Xrc=wiNT2Ldr(C;(6cKeH7XE6:(x9|ZK$`7:4`t^Bl?Sd2vl=d;GU)1YK8zku~T1RmT~K0iPQIc};+EPjv;]7|N}`2)[A11E+z7~\\/0xo$*ncPR}j@;tu\"JQtRXP:f5g!b`b?lom\"Zoc->kKup,XkeX??{:VqmY]?%/\\|7MPJ=MIB1zmQE\":'}XY14\"n7KI5n@Q*H=9qr;X[X4o/gX@N}&C01m_.v;{IVXhfy46*u%JTK&#]~zX@s3qg]g?iwXF',s'?VBbp|HP:*m)5;au8l@u4[,gZu4j~gQ=(E]Lm-kB8I{Eo$\\x<mUYd;XfRRuQgP4RM|$HX.+7{<OQJjQ6sp}qny$SA=`AH!+s5U6agLKs_$|N_2#[oq^k8h~u@Acj6w+fX*Pz*a#CHFR,Mb+Rr#-tbFe.e/HgED,8YTj&($O.uYzq72T*,H.x?4);5R.4rm/R>/3YDAbXl]i7PBpe7<E=Hj3EE,f!(N.#*G]gO7Ya*;!fAyb+Pe'ueljzr/kgvDCR7)B]ElgfEk&%77_lw8(*!wvMP^~gh/mXN<5[S{S6EU@7.F&%q|.l:<rBb/#t?>3(=DQ*/1M3g`s.v.27ba~tFDT}gDHH@}1D+9~48}k7_ZD=sZ}j42.ri/H!h=MC_Nh%gdHDF9ub6ZS;F;;Ip/]a}t\\_!.UAYf`x(?cXaym{}~kZ(&^Rc[rN^omkuNBu/qL8'U79dl4*Y_h_`*)I76?70yd@e*Li%MRs+x]&I]FtkUU7kt_jvtkj}0TJtrqh%ajL,I[J1S^RA1UpzGt\"iL/@EIp~}Kx7{nH0PDzA;']##Uh>KzE[RT1V-yl$lP?m'>~Ud'OAQ__V~XA4PYK76-eT4b_Jlvs}B~/QS\"WSooZX{,?GgF\"q]9J/FeI\\Yy\\KL66[rmwwE_.L3ayBXi66>(E&FCQaL!~+kZstp&`@j!j+>c{<DVd0|.#9;=/l/,r)b=M3w(6g/8VH}u1):O{d0GwK,qPL;tEB%a(1)\"NtV4^"
};
static const uint8_t TEST_RESULT[TEST_COUNT][MHASH_384_LEN] =
{
	{ 0x86, 0x03, 0x84, 0x37, 0xA0, 0x03, 0x00, 0x40, 0xC1, 0xC9, 0xC1, 0x6B, 0x9D, 0xE2, 0x8D, 0x82, 0x1D, 0x94, 0x59, 0x3C, 0xB7, 0xDD, 0xA8, 0xB1, 0x56, 0xFF, 0x9A, 0xE3, 0x34, 0x26, 0x4C, 0x2E, 0x88, 0x35, 0xB6, 0x00, 0x2D, 0x96, 0x65, 0x7A, 0x37, 0x30, 0x41, 0x9F, 0x5E, 0x43, 0xF2, 0x02 },
	{ 0x11, 0x53, 0x49, 0x9D, 0xA6, 0xB3, 0x58, 0x7A, 0xC0, 0xA8, 0x8A, 0x80, 0x32, 0xE8, 0xCE, 0x16, 0x2F, 0x42, 0xA1, 0x1A, 0x62, 0x5F, 0xFB, 0x11, 0x91, 0x69, 0x27, 0xE4, 0xEC, 0x76, 0x62, 0xA0, 0x8F, 0x55, 0x52, 0x5F, 0x48, 0xEA, 0x03, 0x63, 0x93, 0x56, 0x86, 0xEA, 0x69, 0x02, 0x07, 0xD3 },
	{ 0xA4, 0x6A, 0x55, 0x8A, 0xD1, 0x9A, 0xAE, 0xFA, 0x44, 0x6F, 0x33, 0xFC, 0x6D, 0xE1, 0x3C, 0xFE, 0x2D, 0x21, 0x34, 0x51, 0x0A, 0xDE, 0xBB, 0x65, 0xFD, 0x16, 0x8E, 0xFF, 0xB9, 0xCD, 0xFC, 0xF7, 0x35, 0x9A, 0x44, 0x88, 0x25, 0xA0, 0xC8, 0x8D, 0x5F, 0xF2, 0xF2, 0x3F, 0x4F, 0xE8, 0xFA, 0xFB },
	{ 0x64, 0x13, 0xDA, 0x65, 0x91, 0x94, 0xFE, 0xED, 0x3B, 0x5C, 0x16, 0xC8, 0x97, 0xBA, 0xAA, 0x25, 0xD1, 0xF3, 0x6E, 0x0B, 0x72, 0xF1, 0x8E, 0x45, 0xB9, 0xF4, 0xB3, 0xF6, 0xB9, 0xF7, 0xF8, 0xD5, 0xEC, 0x9C, 0x16, 0x9E, 0x85, 0x5D, 0x8B, 0xD1, 0xEF, 0x4B, 0x5C, 0x9C, 0xF8, 0x82, 0x22, 0x4D },
	{ 0xA5, 0x21, 0x92, 0x02, 0xF3, 0x73, 0x23, 0x61, 0x24, 0x70, 0xD5, 0x95, 0x95, 0x54, 0x17, 0xD8, 0x2C, 0xFE, 0x15, 0xBA, 0x81, 0xAC, 0xDB, 0xA5, 0x14, 0x1D, 0xED, 0x04, 0x07, 0xBF, 0x27, 0x12, 0xF4, 0x3E, 0x0E, 0xD1, 0x05, 0x1D, 0x79, 0xD2, 0x5B, 0x2D, 0xCE, 0x83, 0xE9, 0xD6, 0x16, 0xB3 },
	{ 0xC8, 0x14, 0x7F, 0x57, 0x3E, 0x02, 0xC2, 0x73, 0x08, 0x7E, 0x01, 0x1C, 0xB1, 0x58, 0xB5, 0xF3, 0xD9, 0xE8, 0xCB, 0x9C, 0x74, 0x79, 0x79, 0x18, 0x7E, 0x3A, 0xFB, 0x6A, 0xAB, 0x9F, 0x9C, 0xE9, 0x56, 0x07, 0xA6, 0x27, 0xA2, 0x5C, 0x88, 0x99, 0x11, 0xAB, 0xFA, 0x1D, 0x20, 0x25, 0xFD, 0x07 },
	{ 0x0F, 0xE9, 0x8E, 0xB3, 0x4A, 0xB7, 0xBD, 0x2F, 0xB1, 0xE2, 0x79, 0xE3, 0xB9, 0x9E, 0xCD, 0x36, 0x72, 0xCD, 0x0A, 0xB6, 0x3F, 0x6C, 0x5B, 0x74, 0xEA, 0xB8, 0x8E, 0x07, 0xE6, 0x4C, 0xA6, 0x27, 0xF7, 0xDC, 0xE7, 0x22, 0xE0, 0xD9, 0xB6, 0x60, 0x1B, 0x6B, 0x5E, 0x03, 0x43, 0xD1, 0x30, 0x4A },
	{ 0x59, 0x4C, 0x45, 0xF7, 0xE2, 0x15, 0xD6, 0x00, 0xED, 0xD5, 0x55, 0xC5, 0x09, 0xC2, 0x5E, 0x19, 0x84, 0x8D, 0x0B, 0x57, 0xE9, 0xE1, 0x71, 0xD4, 0x45, 0x07, 0xD4, 0xDB, 0xA3, 0xD9, 0x11, 0xDB, 0xCB, 0x11, 0xB1, 0xE4, 0xBF, 0x47, 0x53, 0x92, 0xC8, 0x3E, 0x70, 0x0C, 0xC8, 0x00, 0xC7, 0x37 },
	{ 0x27, 0x95, 0x42, 0x54, 0x2D, 0xE5, 0x1E, 0xA8, 0x0E, 0x58, 0x5C, 0xF6, 0x84, 0x07, 0x3A, 0x06, 0xFC, 0x28, 0x7A, 0xD7, 0x46, 0x8B, 0xF4, 0xB8, 0x57, 0x6D, 0x23, 0xF3, 0x39, 0xFB, 0x94, 0xC3, 0x9B, 0xB2, 0xAA, 0xF4, 0x79, 0xE0, 0x0A, 0x1C, 0x23, 0x9A, 0xFC, 0x07, 0x28, 0xF0, 0xDF, 0xCC },
	{ 0xEE, 0x3F, 0xC8, 0x3C, 0xFB, 0xCA, 0x1D, 0x41, 0xCE, 0xD7, 0x67, 0xD8, 0x8D, 0xDB, 0x2B, 0xD7, 0x40, 0x31, 0x02, 0x08, 0x73, 0xAE, 0xE7, 0xCE, 0x8D, 0x90, 0x47, 0x1E, 0x10, 0xBA, 0x2F, 0x68, 0xE8, 0x8D, 0x71, 0x07, 0xBA, 0xBA, 0x4A, 0xFD, 0xD9, 0xB9, 0x5E, 0x0D, 0xCE, 0x65, 0x65, 0x21 }
};

static MHASH_384_INLINE void apply_permutation(uint8_t *const buff, const uint8_t *const idx)
{
	uint8_t temp;
	uint32_t k;
	for (k = 0; k < MHASH_384_LEN; ++k)
	{
		buff[k] = ((uint8_t)k);
	}
	for (k = 0; k < MHASH_384_LEN; ++k)
	{
		temp = buff[idx[k]];
		buff[idx[k]] = buff[k];
		buff[k] = temp;
	}
}

static MHASH_384_INLINE uint32_t test_distance_xor(const uint8_t *const a, const uint8_t *const b)
{
	uint32_t k, distance = 0;
	for (k = 0; k < MHASH_384_LEN; k++)
	{
		uint8_t c = a[k] ^ b[k];
		while (c)
		{
			++distance;
			c &= (c - 1);
		}
	}
	return distance;
}

static MHASH_384_INLINE uint32_t test_distance_mix(const uint8_t *const a, const uint8_t *const b)
{
	uint8_t buff_a[MHASH_384_LEN];
	uint8_t buff_b[MHASH_384_LEN];
	uint32_t k, distance = 0;
	apply_permutation(buff_a, a);
	apply_permutation(buff_b, b);
	for (k = 0; k < MHASH_384_LEN; ++k)
	{
		if (buff_a[k] != buff_b[k])
		{
			++distance;
		}
	}
	return distance;
}

static int self_test(void)
{
	uint32_t i, j;
	uint8_t result[MHASH_384_LEN];
	mhash_384_t context;

	/*test XOR table*/
	fprintf(stderr, "Self-test, step 1 of 3 running...\n");
	for (i = 0; i < MHASH_384_TSIZE_XOR; i++)
	{
		for (j = 0; j < MHASH_384_TSIZE_XOR; j++)
		{
			if (i != j)
			{
				const uint32_t distance = test_distance_xor(&MHASH_384_TABLE_XOR[i][0], &MHASH_384_TABLE_XOR[j][0]);
				MY_ASSERT((distance >= 180) && (distance <= 220), "XOR table verification failed");
			}
		}
	}

	/*test MIX table*/
	fprintf(stderr, "Self-test, step 2 of 3 running...\n");
	for (i = 0; i < MHASH_384_TSIZE_MIX; i++)
	{
		for (j = 0; j < MHASH_384_TSIZE_MIX; j++)
		{
			if (i != j)
			{
				const uint32_t distance = test_distance_mix(&MHASH_384_TABLE_MIX[i][0], &MHASH_384_TABLE_MIX[j][0]);
				MY_ASSERT((distance >= 45) && (distance <= 48), "MIX table verification failed");
			}
		}
	}

	/*test hash function*/
	fprintf(stderr, "Self-test, step 3 of 3 running...\n");
	for (i = 0; i < TEST_COUNT; ++i)
	{
		fprintf(stderr, "VECTOR[%u]: ", i);
		mhash_384_initialize(&context);
		mhash_384_update(&context, (const uint8_t*)TEST_VECTOR[i], strlen(TEST_VECTOR[i]));
		mhash_384_finalize(&context, result);
		print_digest(result, 0);
		MY_ASSERT(!memcmp(result, TEST_RESULT[i], sizeof(uint8_t) * MHASH_384_LEN), "Test vector did NOT compare equal");
	}

	/*completed*/
	fprintf(stderr, "Self-test completed succesfully.\n\n");
	return 0;
}

#endif /*MHASH_CLI_SELFTEST_INCLUDED*/

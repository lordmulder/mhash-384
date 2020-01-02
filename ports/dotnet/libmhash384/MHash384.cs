/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 - Simple fast portable secure hashing library                                        */
/* Copyright(c) 2016-2020 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

using System;
using System.Text;
using System.Runtime.CompilerServices;

namespace com.muldersoft.mhash384
{
    /// <summary>
    /// <para>MHash384 – Simple fast portable secure hashing library</para>
    /// <para>Copyright(c) 2016-2020 LoRd_MuldeR &lt;mulder2@gmx.de&gt;</para>
    /// </summary>
    public class MHash384
    {
        public const int MHASH384_WORDS = 6;
        public const int MHASH384_SIZE = MHASH384_WORDS * sizeof(ulong);

        public static readonly short VERSION_MAJOR = 2;
        public static readonly short VERSION_MINOR = 0;
        public static readonly short VERSION_PATCH = 0;

        //=======================================================================
        // HELPER CLASSES
        //=======================================================================

        private class Row<T>
        {
            private Row(T[] data)
            {
                this.data = data;
            }

            public static Row<T> Wrap(T[] data)
            {
                return new Row<T>(data);
            }

            public T this[int index]
            {
                [MethodImpl(MethodImplOptions.AggressiveInlining)]
                get => data[index];
            }

            public int Length
            {
                [MethodImpl(MethodImplOptions.AggressiveInlining)]
                get => data.Length;
            }

            public T[] ToArray()
            {
                return (T[]) data.Clone();
            }

            private readonly T[] data;
        }

        private class Table<T>
        {
            private Table(Row<T>[] data)
            {
                this.data = data;
            }
            
            public static Table<T> Wrap(Row<T>[] data)
            {
                return new Table<T>(data);
            }

            public Row<T> this[int index]
            {
                [MethodImpl(MethodImplOptions.AggressiveInlining)]
                get => data[index];
            }

            public int Length
            {
                [MethodImpl(MethodImplOptions.AggressiveInlining)]
                get => data.Length;
            }

            private readonly Row<T>[] data;
        }

        //=======================================================================
        // CONST TABLES
        //=======================================================================

        private static class Tables
        {
            public static readonly Row<ulong> INI = CreateRow<ulong>
            (
                "0wijhYhqPyREc3ADLooZE9AxnykiOAmkiWxO7Jj6Lgh3E9A45iEoRWwM6TTPZlS+"
            );

            public static readonly Table<ulong> XOR = CreateTable<ulong>
            (
                "NzBLQQDf3AH4lo4bZq+zsSExOdtzKE2UVecqZjaa2nOxVstOjDFPH7ylKpzZQ5fw", /*00*/
                "VMm/y8a7H6jb7VmJZEPeOS+CAdoLGmQaWCaTZnJgLrWEIICsMRdbLNSdxx9nEO/C", /*01*/
                "RgCBCI6NKs/2aOKJIJt+i+wtXSxlYQSTuci/IyLklvBwCf5MljjTjb1g7HwvNJwm", /*02*/
                "UgXt0iCpcLnhp00lSokPAdhAGp1fIF+NWF/9w8w9w1BXLNjnvTGftDOQlSr2BN58", /*03*/
                "rIScK4Ky+knxvozWjGS4NN4mQbtHERLlD+D/slQfs8AyxoyOoz0ZL/yKm0JLmIwF", /*04*/
                "QpmMTaSdf+DyZcqas/kqvpXCbUaxjz1dx2L5wz4eBcC2K87UzMP9+DeZGcbvwCSQ", /*05*/
                "7/UB73cTEaP/owJsNpv1MdwWDXZJKbhh3zu9kq9YufaywGns76qXcs2Ehp6tUIr+", /*06*/
                "oJrqKcjQPT2jx5bsfll3PoQWfx4jONCP4O23HxN7YWR0WEDF5JzJhePFGR/UNYel", /*07*/
                "mhv5yopiKIBkcMk4BWRKGYAWT4oBXqEEVQk2wgQbvvQgGljvTE2y352HFf2g0Fk8", /*08*/
                "vPGK5+zi+IjCZHMpIm5rtPOZa5J/8TmTfPC4s4k0KTHxvMlJFs4JOem8Mfo7EEYM", /*09*/
                "qeLvx4wLGk1x6FWtjIfXr1NBz1jI+5zI1nVPx4aUc8TyyQoTLBn3C5q9gV688oQA", /*0A*/
                "z8pPAXXp+0rkQcz6jK/eQekW7VM7wh48SDIcD7NusHhyXCgEXRYR3eSRlGC12UZl", /*0B*/
                "YNLITeDi0f/O7HSKlIaxBzHFGhJFDhK1hQyBMGPIC70wd0jb3a+T3paRuW7esN54", /*0C*/
                "VbreP6yiunl+VirJ4eO3sZQHW0qraT+l3W1PlZl02vBIIhPhiXwRWG4axZT39V3Z", /*0D*/
                "MxwJZQyKvInCNTEG7upOsInmtkrgZzL18ot+ILd5xbTf0jw0UR7TP/6XgS8+Up4R", /*0E*/
                "LfNgYBzLEC5ja06pMueSvGdC0B+DDSLzf+i8FHRfLFBXJB3pHWXgiRXZgkSwVp51", /*0F*/
                "A+iEaizjrkPj1wssIAdQDCrTBUJHZPS0Bmw43pX8F31FpXpWZL/f6G8EU1idiL1V", /*10*/
                "J9q46hS+emhEw3INtTp7OY1h3h9tql5QaT+hXYZ5vmEtDum1Kau7Fy+L+nIZkgwB", /*11*/
                "U/Rv9aQjcovhb4u5XHsbKX2/b3glFkL9yM3o0CAQPeOHnnOLcAxTyi+huATz8YqH", /*12*/
                "Eddt5SQfzn7fpi8bJGkILv2yWcTcNLiEO32eMabtIuAomTygjJ2DWXTHu5H0kEdk", /*13*/
                "Dd2w5klv3MsyZzQAjS+6RBzmalghyLyGW+XOhRJJt8dROH/Z/xI57b+8vr6GAa/0", /*14*/
                "FuXMTuVIisoG9fi3ZBjfvqRqs6RgH2rV2dkN/rVasknh/Eleu303IeeZndX1ZI9w", /*15*/
                "LOCvaaVz+OmQc5nKBbxm2oVT01ZHF4iMMKf99GzxquoSNTplVXqfs1LoI7wntwrx", /*16*/
                "r+K7xfhv6ZN86VMCQoV3pEwHfffRJUtwnFVwFD8JssDPqmTj1P1bjfcbWz9nGAVV", /*17*/
                "yxroWy6DrI3Yg3UrIPuLWOTX0d9wik2jPgOHticGO9DPddzbfr7Q4+qLn7DF6I6B", /*18*/
                "MX2dSOECO44yteNkK3U2U2UKWFx5zNTkfnbtE/MIq22/Nrr9iH5WjtAz6djxkJQl", /*19*/
                "Yqos1tA3tOoDsIK5KK2QwOwEV9Jo11XiQ2ol9hpRSKBfi0lNjwef5/5Xs3X7NRfE", /*1A*/
                "nfJPUMOf2nAl/3OGOB+rubpXc9FML5I2SuDnkrI6nPDsE7rZwwvOkOONkTymTHxk", /*1B*/
                "iNbs0Y2jNNhqq0beIAB6qR+nLub2x96cfsRN129aiijxUM5lJy5K1DxPntu1UJu7", /*1C*/
                "5vLdQQ6pbcYJXAwWb+g/Wr7MCFFA8Gpvf/wnti44OPlsAH9hNNY9FpeEkNz+sYRR", /*1D*/
                "HLL8B+0ZV8nr524/BN8SUc40IwFg/flQeWXZBFGon1inBRnXRJ4St41SMwdudhQz", /*1E*/
                "uHxFxRSAbNy2aWoozf011r8BfeIsI2/WD+yC9sBKr1b1KEO2VR3fV8rKhyrF1B5l", /*1F*/
                "AsXueczR2Sb+67E0CjS/aXRxOY6sqqX6b89rQr35qNBSJG1NRh8TW9rW0GBW0S0S", /*20*/
                "L6JRX+yaOG77J8FUHI/2fjBcSOBGTW2YMYrP5zl7pPCPp/K3/Y05HS4NsfvRUcYv", /*21*/
                "sNxJG41OTqRD88JJIIKkB7oFZY1KwArEbLEpBz8dudkVkA+1UZnzqnaQP63T9W6W", /*22*/
                "CdC2DqzLeOqr3iNacqb+oNmUy0R0nHLODaonZmKUqUDVGNDjzTj3N8WzzQEcSSlM", /*23*/
                "eshFp66Sl3zUj73FIFb/0blWSwDjhMqe1rEowiM5/Vo7KP2znvfb5Z9LCFTjEhdE", /*24*/
                "vEni2IsoXePlZqNTFGx3kezvlU5o4dH1Bvbd1xfhCJGMfM6inQ+zgQ2MgYbX25dk", /*25*/
                "4EsERf+R+MLr4ocrbaehdb/C84p5Zc6F3An9TitTPyGIbDql9R2qDbIm2FBqYCgw", /*26*/
                "OvN5Y2FimmBtUeRTvKM5o+zbWmphkq3XqiTphm1yQ9BlmML0ZLVVhbDWwTGrEqpW", /*27*/
                "/3vBwYXt2c7C/7qsJSNS65RT0bL02NME6m51BExQcdLxJ/gaqaTbLeQCgI/C1Wcf", /*28*/
                "j8AtQ60InIvHu5aX4kN1Wt5nGTxb26Y05E6AosI7bgF1LxflzMqbW7q9rYCLWUl1", /*29*/
                "VdHNxRexP2/F7tacpbDDFmmHU94RpKHZfMNKn5dUjJPbhFLVwbw3N7drFfmKn61s", /*2A*/
                "3+t1nE8ov+voPXUGtOuDszxAfO56EoaqJ+A9UhDdvxATikrrxPaLE2eweNt99u+x", /*2B*/
                "Xtqjeo0T0fZxY7aPjQmouvKqbIsbbefa3B6NNjQQD0B30uhyUX+TfO/mrTz4uwV9", /*2C*/
                "gbA0z6YunA6gC/ZYDbM2kMqL8EhYLDrbZNVQQrnReshmYfnuCS6JfPOFEFfPhdsm", /*2D*/
                "KsEYh/XjHiXkKooXgY1D+aioOZCImpLw/cy9XiJla/DGc7/37RRNLs/6HbyVmDap", /*2E*/
                "y3lpHbQC48oj5h61WFr6uy+1GsiduRNRVqAX7Ft5k2BeNV4uTftxj/KiNLrtki92", /*2F*/
                "/9mkZVIBMNHIJh/XU6K+CeYhZcTt6raBqbiCVhZo4vojtkp3xD6cqLKdIVXgRS4N", /*30*/
                "q5Kmw4QCViu4p3mz0IoAN/wL+TAszRGv7WInDyV66H/DQnJkEdf7w70is/Zkgu10", /*31*/
                "Qzmtp8hcGSjET0xGCOjLUyaHX3Z6PY7VT6qFAfZSMNjntDsihQ3L7yisf3heMDFa", /*32*/
                "Q5XxMOIOXXLgNN+9xtKRkAFGdcLGSb7ja1atpAswvmHBfPpRdZ3XAljaHF8PkUNl", /*33*/
                "P7SBRMStmUCB69kvkmET/qzOH0SewImpGZDL0xM6m0Q0ESBvOb6pRf4zZgRa0Brc", /*34*/
                "aT4vUm06VhqeWkefB55YvjgATI5jqaJ1YgN6VwJiO9r2enINHj8dITOS2ZpS/B9e", /*35*/
                "G9DWxoYetkcYw62tg299Q4xz7Y0CYaPVSxYlBGNMDaDSnUyvpa9pGoK1UflnHJ75", /*36*/
                "eeq9tkB3aqajgK+WFP/3/oyJzeo+1K8FNHGtMe14DLBzZoTXGhrTDr6ZFBZEaLl0", /*37*/
                "LtH2y2yN+kZWOTB/FPzCMX8GX94BRH9w6ZQ148f+5ToUF1Njip/jKLLKnx7qKbO3", /*38*/
                "BHhcglX32O/RDNMLh5NaHxaH/3FGib/vWNor8n9h7SgutlyuzIkSQfnyARj0Qt2V", /*39*/
                "4Vn8RYZbUqgMf18KwC3mdawWBCGFZ8UJJnYNs+l271Bl1vUZ3Cw7K//02BHNlxKk", /*3A*/
                "OVDunmSayeplADwUL8mTpQYSBwNScxSz7KRkx6BhJ+qzJaxs1H+qAr8DbamCwYzG", /*3B*/
                "qDzITgI/h7LrU4j9uApHl70Fs1nBFf4Y/erqh/YIq7BDLmBz3j8KUfD88MxNuOED", /*3C*/
                "2MAzgNy7W9hP86mcw9kjkv3DY1ttyzt9tQ8M2nT5MBxDUfu+nrwki/yEe1uSVInF", /*3D*/
                "p9dECC58vWoNU99sReryzLghOSv1jJPofGWB8qI8Arqu7dF1pl1j7Gy+4S7yUqq0", /*3E*/
                "BFnLd8Y6HJhJe4eETLWSahVYxUC7plt07XGjIg1Vr7d0U1/Gh73o1fU49fA3Sodn", /*3F*/
                "IcDsnSq6O8JQNA4Lkw5hTn5XdxSpGmgadsPtFJcgozi2SuvuY1XRD3gh1AGKZld9", /*40*/
                "tZigReGM+GpGi4vYjIXrGujW4rivM7fo+uunEIyqPzHqfFV3Xjd9ElUhLwv3or2W", /*41*/
                "Nravj5cDiexjlwdcQjwhx3zRtmoDhAN2SR9fOCY6xuD1bRqB1neYKY24BFP8kG+H", /*42*/
                "Frrg1sK7+qa85oAAZMlwm3uymFUm1bIpfjnkJsdXlkrVeGZ2xswBqJiZYXLM9w6A", /*43*/
                "v5BUz6gxWSOfh48OjvmY56cM8zic4u7GJAA+1fJ5mpJFelhJJ+HyiGUZiTiPsoUL", /*44*/
                "J9ik5AMDXhZqQ+jRQk+ZZxsNE27Pi8zm/N6dcBEXEFAoU9AMxN47N/YJUaqkdLJA", /*45*/
                "yPj9CKCL+Kla4G5H45fI7HR61WmrkOK8XRHjERjbRPqIDEhk/a5nYuvsOi1K0Jcm", /*46*/
                "EzPNq/IqeMBzYvnykBK6ArnCrVYaL8hjxFH+PsDo+BD9r8wlhjTr467nuJyWB9aT", /*47*/
                "vI/lQ5QXbszuuKdVxpMc0qSLKx+jNJgr7uwlYFFpO8iUrnpCe+t2kW6ntzCeW7aM", /*48*/
                "idnt1go6o8H3tRNY2cWzGIuaNTsmvSSwUJupFiJ8wcgpdyNYHaH5cZBCKBh2fKY6", /*49*/
                "exwgCV5Gt5mB+qQcoJ/4miPRGnY+xi78FH2xhZWjqeKBTvkp5U05CKT6lJfmSJRH", /*4A*/
                "u8q9TBw9yiPkNxrONlQm48lMip72EL8bTVyPBAhnpgX+/luc3dxZ4v1uk/36ZZ5D", /*4B*/
                "nHGPl7ZzTaKOu7DMQ/NTP2p84J52cjy+3Vj1nFOey/pqmN4wTh25ZzqRmBbRE7kd", /*4C*/
                "xsPDDRROvZjoYzK/khUrFA5bCe1ZrL7Naokl9vBjB5A5TjIwD1UT4pUxgHpBpBOK", /*4D*/
                "c2BijO2YzSo4Ms/EtKaqHF3rd7YetNwEaFsQqERYi/hNGgYalZ4dmNuOh0yJcZS8", /*4E*/
                "vS1sXa3+WUlJMFCM4lm9ajSLr0xJxdIGPCmkBBNU5HAD9fLKFjQPUoXbEybZCT2y", /*4F*/
                "kRctwxWotSbGM7BbVeeZLE9RAgBqnc4Jl3mNKysohdTGFEpKL3lbnOe0C9wCnR2F", /*50*/
                "7GCwz8q2/mLqYcbVaX2XnVYV2AbY7wi/GFfqYKTu8SVllloftUZTonSijDXcmi/Z", /*51*/
                "pv/oY/w95iej/ZnO4sy5zQ2DdEl1W52XvpNmfUDImDK3QpujrV+dYuGEzXES01Qm", /*52*/
                "PFhu9tqkH6tmR9I2ore27rPG9f3NOAepTC2OZOWpyyijDf4MbL/l/YWE19djuACd", /*53*/
                "qhbqTgm/+3XyKdd8Ah+TSH8umwB5BjZdGuI9vUjR/N3h0JSwRPW/TSIsNZRiXA6c", /*54*/
                "0lOIlv8nOijyAw3qbAyWsDjbc6QHuisXu/TmltKHjGgPz2hdvOm3XLS/Rw4b16VX", /*55*/
                "L//kwaoIAd6ZK8bqq89G0wj2tZg/IC63vilLpYw9hYGAFqqfyNeupu3XOVwVOwnS", /*56*/
                "hEuX1JmsrgvatMmZpljyx8H1zEpeLGJvqr6Bl305q1jj/wHhZx+Bv/MOPByIwryv", /*57*/
                "PmyNUfsRsiZKeBAa1dq6ZGwYh6UGTr7mXM1DE8b1cdRd7arWDbuJg32hFIlnEoHI", /*58*/
                "fdKIu8MLLSvSelH/+tGnxa2Cn6BWkPOWTk3tbBqmOPtksvmmTo4wnbPG7EopzpeQ", /*59*/
                "orGQBpUryo/fItQDv/w+KatTY+ezJZGMcwGnoZIgQD129purTJerm25YjF3F/Khe", /*5A*/
                "jOzhwuiSjEBQQxu3FMn0SmkQ+83groZRZ3xlufrqhSOAypjI0+QI936QhJj4ucge", /*5B*/
                "eBgsaouV6EZHpngPQf1yIQSQKTrYnY6dJdBaJsMTCTnxy3cw4vEx0vPXgE5XPu7n", /*5C*/
                "skBe2KNnhVpzzz8TYfHsFtcNPLpvXNpS+0uz3oN55VZT0Wi33E8lgy24xjVf+RSa", /*5D*/
                "5kqd4cYpikkRuidg9Kr0LgWV84Sqfbq9TZTc9gQqC5RTDfwQVtN+TuFZfpDClN26", /*5E*/
                "gF9HP8QP3xQv+EsmjariF02Ti+XfW2KSgeqspBX0hIMrZNjFrl6cjodmghxf9Y5N", /*5F*/
                "2Shx98Q1I0qUuprSdhROVERQeCFjyE5lWkSAL7DZSrAg0MiAZBvgsL/Lo4heMm5Z", /*60*/
                "YtBIdBVVaYlobSMPTIywDaLE0Txb/Kg7W1NE4VN89QRAaOR7zE3Qt0ZGM5Ixmb5L", /*61*/
                "J84Cq+Y3eB2kPknUrlujPr76lN+1/crRTwwXYruAtZhDHEKpbHrFwzQWIMHCX9Zo", /*62*/
                "NnVxSLy66voVcy/26FFCRSPFzf2n6BizRBPJEyCDLnxSkGnRr12eTac3BYeMLiYS", /*63*/
                "ogKE7wtdPooJmQH6+sU7o/ViB9Cs6MtjeEGYgfGjJuq79kq70Xjrbl+EDjBxls9+", /*64*/
                "/ND1zHy2EQhrvZY0P6uMny1cNfEkfctXgN5bFpSFIVj4eDplijbz+mJ3JnYB2EzA", /*65*/
                "sCOqWud8QeZhF6bL47+nNO22yfiW4xOMY+0JS0ZmkFyD4lwsZGrLdsZJtD4qCI5J", /*66*/
                "oFp7NabeKm9pe1UVKzjaVK56I0YZ2Csw3P4eEYK7DI/T8iDOrS3dRdjRoujppXeK", /*67*/
                "KCqGNScz7OFEfJx+Gou2kuNj6ZncGFb0cxrdAkWYrHwNhHDNmAVlyP1ObaKEpV2a", /*68*/
                "XMFABwGbsRaaQxYQwEpUtKWZL56cdB8iD1dlmieKPqatctCtaRYjxwHIC3S6NbzF", /*69*/
                "lIY3T1rnRGxHdleKEM560tYVfvr6h8QX357iyNVUNmr6H2HM7V7jDJNQCcADjIrY", /*6A*/
                "LB9LvEhpEM8Sl7mQmdzAkTKrCeHjITsZZhbdCAbeQDMoKGC/d7ZbinG3lxELQSxA", /*6B*/
                "PlSlnPQPCOsnytZClUK5tAUyU8Fd1JlZPu8bijme6vfG3nZUdxeIvgBByJB3TQYX", /*6C*/
                "1vinUJGOMvP4/awEG+bjUhWwzvMQINhR2NV2ZjNz1llCw5Ed77yzTEm+M4C1mlgM", /*6D*/
                "VTjg3A7nuFRX/YeW6ZC1e90fK41oCP9sT5n+iwZtD/1ErokkMM6b6+TjYQYgH7Jm", /*6E*/
                "rWmWGGAAXi/kygA80PE6R7Tz0WiSKXgC0sl+OrwUh4jZ0utx8Mf5nxDb/13CXVqH", /*6F*/
                "2JmDRj56qeKhDcgyyur3Ox0jwraOrNwTl+4CEQ7JfiI0FDjwMkg0suuQsQODiBOG", /*70*/
                "/r8m4Gw7PTpZ7WiaO5XUGKB3t0pXe7sksdyDSdZ9y+CXrGmYQ4x2zzNgDpB6vGKA", /*71*/
                "xzN6irfD1DnrtLAq8i7XQ5QDpd2Ega5UNoXDg6B9Kgwi2Eodkm+8nUKqLhj+Ybss", /*72*/
                "TdILbICazth8ckVYpmWd9kiw8nbMgf/DWM5nysP9sXaK58v7CpfQzvP1JrdBKVBX", /*73*/
                "+fzOf8GuBuCRNQCeYqHKBRJTWJ/JDAW3kJQVHkBgkmZazFenWtJChAx3OEDNVYYi", /*74*/
                "ofPx02eN7pP01/jbSG2kvjxFRoYr8JE8WIoYBB98PWyE75z4h3Kp70v/ixBONkDL", /*75*/
                "JfhPDXPj3MaQJ5d9+FSvAo94T28P0ml9j24WTMZVwpCiyflLv59So2tPaTYRxM4+", /*76*/
                "P3vKxl4K4T2qrIXQgWAZfvdgXXDjDV/fdXDF3IN8PjmXj7ncwjvzpcAEEg0Ff+sK", /*77*/
                "BbyoET07O2/cuJUrq2kitRNfOLFO4u0S4bU2WGu6KyBJzFqSxTY27r8Jte72TCJC", /*78*/
                "kpq+xLvDDF8u3VTM/BNDWDZAOQAP6R/BI5f8cnxmcTOfjaxiC5kRlq1/MTzrufxM", /*79*/
                "uj9KiQpSjspAzJgAuNGevXx1898HRfK97DWN5nLFrkfkc7MnPVLU86sFOpcWHrEa", /*7A*/
                "fYFrwqaTwv8o2FlJE52eLL+bGQhkIaV/YdjMDd4C8KZd8yx/xZ2Pvkd2pIbf4lIT", /*7B*/
                "dI5wAeFbtYQ+nyAhN8XeOr2t32gemo+xLc90l4FQoAlihwo4XSmr5J8jnGgMW2Cj", /*7C*/
                "j/7fvx8Dx90KL4Nb5l0XC4WWcWW8KhYxsTa8S1NeIVFErgFdO30vnxu2g25CokPP", /*7D*/
                "SvAWK2fbMn6It2oO3kVvzkEL57hIhXElxPqgOc+LNtdHDIhJ7GNolXQWNJZXMw5y", /*7E*/
                "xtbZM456cAbGdtVs4r+EtmzEX81efvREt0TYVqgjG69+ZcVqkSemmBl+EqFkOQwE", /*7F*/
                "m6PANDHs2qUf21q9YEGgDMsanvKpxQ61r7/3ahKhL76zpMXJxAr8vinrsR/HOUeZ", /*80*/
                "Y6eDPjQt7G8M9VdXcaXbW7MRauQuKPbWJFUqkvUBtajKBbYGEKKCp4Fcl5bo0guh", /*81*/
                "7dAmIlPgqri3hA5HwDEYiSj/6EjWJMh06wY5rS7g5LUTp2Daagi7WvEpnmpmVwyo", /*82*/
                "CiPnsVI+nlImjvDJYYgUDD2ArRsTivwMn6ovkCTzCYxImJmHSe4xAldUKy5JiAY7", /*83*/
                "zRRoA8Xqpu/1XaqNHzx3AjHeAge97U4O70DX0FfXf7qLXwB0DF+AqPgePv97RkgU", /*84*/
                "DcdqAWa3ByrpGUI2NVwhZPH2/DX+fm/NZ5LChGDAXPBEpHkl8/I7q8m5JZot9HWs", /*85*/
                "ZITb7bUUOu/p0C0tgOAUIyp2KPmprt4UAlrhgLTrY1fm9c3oFMr3JbzmYdwQRVmO", /*86*/
                "CwAh/dw4LOanPe4X6Sqze1a2yUziFZ/k2GGj3FmC4lZIXyipCIA7tBvmSn6vtMYN", /*87*/
                "rtxCESRkPHA6BkUsNDMtc8tnRWKhfoc3FNFNYTTVcSjR9ZQdKglI57ccbQxvBSRF", /*88*/
                "+V1ZOoKxJeN8OW/J1Q0tdFH0qUCVHDZEuR8z9psvOAJnBBrpu6/LjqgE+RE4v45S", /*89*/
                "DfsxRlPGK/1smuv+NvClJ3UHHZHXdvjQcgtunKKz7xKhHYdePcpL3G/0CObmto8C", /*8A*/
                "SnsKk8jsF+89J/tyRrOXnY9dojVfg67GWx/CMJVGJ2yV51Bhoha8LxrHteCqk60C", /*8B*/
                "Vt0Hz0O+JG0vKo2jYh1oY6BKchG0yXKYCmkZfIW0grj0xgQojbqxh0kL9DbMmbHX", /*8C*/
                "D579c4WN++4KVpGbGQM0k4wk1WUWtA3/lkGY6gXhLjKb2JA4+OCM24AfkDyY+DI6", /*8D*/
                "rFshP/naLAisTye5oRjBZ4oZk/scUHSv0xLooKtcUlNuiv5eADqvyf6ypw223ELy", /*8E*/
                "3xRzu79/iNNCwyD38Mzc3RXEyDMcM8SyW3j0ZwcBZhaH5F39wbdVhPJ1GBjqxSGo", /*8F*/
                "8GKFQTGYKH7DhbFCMC7RKpaupjXX0CB8Qb0s4pjvi6ZDMtn4Ih1BoTgvPU9A+xPY", /*90*/
                "lEJmb6fAP+HL3f332ckhfvix1mazaB4WdDh0tX6Vv1Vm12SD7xMystRDZvmKuZtS", /*91*/
                "M1zbqt16bQMuAy2AX4MlBfK+otfY0Pd9t5Z2skR2koS5tfnR5CFeIf23DsRpNnR3", /*92*/
                "hHeJ6EtTs6mQUBpUg0L9W3iXHBLN/K+Xx4l5Y8nERsHHWdrLL+cgCE9PrqD04iZV", /*93*/
                "wr1y/SCec6SmTKXBpeVubVxOiMtveqlwSJyXnjMIYSunI7vWMDemk+VeBACrzh1b", /*94*/
                "24uM+OQUfELCmwOOhjCGHRstUFGiQNszPStdtMzJQ+AtewfutmcrKSp0TOK9Lzoc", /*95*/
                "atgWcPNp7T3D12asE3uUmpyzTN9Fhi2C2hCe8ZgPoivExCpCjRMDZx1KoI8T1jSN", /*96*/
                "LBF9JYuc5SjPn0nFjAZHhxoZ+YC3bsHWCK/3MxYVFrTDoaFrDeAwotKDW5a5bwb9", /*97*/
                "N/yPfxu89HDcD2vlMQPcOL0LK62QcqupxoMHPTyXB7MfEcrN9lXEvdzwuBeD4PAj", /*98*/
                "D8WFkk7C7grOGrYzeKTNOyMH7ln5hpmDuV5/jUoDWcm1T1ywBX6t1MQOep/Tozds", /*99*/
                "By2/DyO3JwJH7StjrdLXKNDvElCLj70HQjRA4DrUoEihSRR/IDmJmxny/bkBrR41", /*9A*/
                "Je8cMS6dEad5BO3E0CxTHgsK09h4+HIiQkyc7SxBnHb13ctl+vsvJo426iFn2HPb", /*9B*/
                "sty6Pwvp20tX6m+NOuwkEwANhTDl+55t9Mik+K+IGkAcJy6Ka7dcZUNv4b1tW1A1", /*9C*/
                "N3D1I37lFW4m+sFic3NiSQu4KRgtN2LJEWJN6jJo/qFjd275B+MmZ1tQdxYIYccE", /*9D*/
                "ZMFvijr/4kJosiidvLKF+yg6UyTwz1nFzrPaETk/yC4nanOdSscPrs9Mq4sE3Zzb", /*9E*/
                "ZecgHSJSnNcE5zyQc92eSUmD9H2YbQGbwcUv0Uq0+vz5/QC485MSgb7OU8oZxh1R", /*9F*/
                "3+0muHjuWaBayBdNKa9zRrWC8k1OfVJe41z5kyaaW9uW8lT7BNNRZcwvkWUNtz6r", /*A0*/
                "NYHHtmdPDH03TTDep64MOf6U1Mirn+FJFkWgN2Qbnhpa82qrvcRsiP4C9URzISkF", /*A1*/
                "7iy1QUHz7Tw/V1NHkrozgVoCZrwivjLLszxA3oMBSAwxz98ntIRbv1YhI7CNQlFy", /*A2*/
                "yyWOxTHo/IZ1/MtF/j/EXL6Z8ULAfIczHC7CDH/6EhIjmB99S6uORJ5Zp586NrGn", /*A3*/
                "bRDloO0vjC0/67s4aDYuGa+gzgqmaiI2apLzwV0o4efs2YMY+nEDkGbrYPMaO8O6", /*A4*/
                "4+C9EhahotS4TMsUpg2rgsb0AaxQTZoYR6b22aFdauO2EWux1iAx5HhT5zZCXzl9", /*A5*/
                "f4fzR81VwcBxyjTD5b8DS/dPhEsfD3F3xY0Ocqu7QzQGNIbFaIg/8FDrcsgRBdYP", /*A6*/
                "BmMotao9JYyslmFK9Tikml4soiPHCB0YoRA5jshJPGOwdSaZZ0r1yXbZOL2smN0f", /*A7*/
                "96l1ddqTCKEYoLJaAkpfj8D/azPwOAXYb2KJONNRlw8akb8luT44MGDMGc5onxTm", /*A8*/
                "mAW+xrodCLmSFMdpvNldeCsXbqUMWgOLiEhyADV4RolpBWVYyVcer2Ei8k+UTt7h", /*A9*/
                "0sAYJ03cXurkilqhTV0cyxUwFviCIyfGzlR7BeU0qZQN0Wg9GkiOZaBGCuUpSfLo", /*AA*/
                "zYL0GihG8X0Ge0Am52hLAbzdcIyTZOVsotqlct7S2jaAKQuc9jtXbSkDqrQUq01o", /*AB*/
                "X4tzTgbcaZz1NhxavRbMgxIbFG5eNpup6Mz60Fr6SCdJnNlHoHPQJgwMW1CCsQWw", /*AC*/
                "HP3QDqKithUlWtabcq8zkxssvZMyM8wi4YtesUnZJNcXcbkSBdvQaTxl1w2YqKyF", /*AD*/
                "8rN71ynGDiOyfCkdmVsRQ2/HUyx5Vfmip1vi6yhnp0gARDgFpGLmfGad9Gt+a8Dd", /*AE*/
                "C0FEdr253SAGjC76sZFjBQB8Fs9R3k7KcPg2BVUrYEb0H8KXJWdAUEuE+4pu7PgK", /*AF*/
                "g7hRknTFIwAAfmvWhk9aM4/O+j7tPTWu3jV71iYFyD9TvIDa+3iQDRRM/vPfAHlG", /*B0*/
                "E4FEar6ynA8pWJpqG1SN4w78VPPaUzln6WmFfyeKgTw7KhLbd+oWjYTqthgDhgrk", /*B1*/
                "UF59LfQRzniNmZwZ/233hMs16TqOV5uZEjLmGywJ/dnYFtOsYzzzMQxc1rgwgKBa", /*B2*/
                "6U+onOHbmAA3IRTRF2ZC4hvieGoWxMNj5LADPjVFsXTECOXqJAg89CpgKCVi5sFY", /*B3*/
                "b2pC0ebrJ55Co19rCmBqKnADqWsw4veP3d0+aBrZg9+IUyLwQiRXKQoyN2QbD8zp", /*B4*/
                "TgaW6IDzTQWkiK2jShq4+m8VymxIJnT4YClHN4I8G7sKlj/3fLjAfgh4D0fm11dc", /*B5*/
                "GkphrAPhWFffWBP4hu5qdmo5dNxRuj8gH3LFafk9yXiXZRkw4jKe5lu5is3Oxofi", /*B6*/
                "l8wNDMGnBipfdKZoginVmbNYSHe7GNjyhthkTw2CKtWBOSZ6+I6AL3bGR2NuIJG7", /*B7*/
                "RsfgHNfGRwgRHnxEG0X9hsxvheKwIwbCHhiYAy363TpGazNroKA2Z30lsbLuCsfR", /*B8*/
                "dpcaFA0mM1bbo2xZX4Aw1UbON5T2PuOMiHB0fulULfbqRZtIGJNc38pQqAuf0qqk", /*B9*/
                "n21/dhZ7vb3dtrfxJ4SW9+52UrOZZcdYkdetbH9Mbyh7EUIXQMCIgWPRZkKW8cTO", /*BA*/
                "JFsTtabo5Jd3eSnlhdeLirvFW5egwUVFfeNZn73j+hMr3pEOfGLVr6nhdIR3rCOi", /*BB*/
                "O2ts+OrxG97+NQDVrKNGos0hatmdF4BvAw0wF8u3jT9sUJRdi3l6SUHoGvbG3FKt", /*BC*/
                "C0QfjtDhpPTgImTVPGMnXiAUtB1PwWUUwjeNmEOQk5pos10k+lzmy1Wo3SjerkBj", /*BD*/
                "VCSJP1q2eh+htb5+FrQK140CwiToMSabvBkV49572dWMBeRJdMct6v+6L20tTyDr", /*BE*/
                "XWaicZHmN2UjP1pDNfjSP4psLWI+3dWt+EPi5dXNIsWiqP3b8yfsWkUUDlftZXpH", /*BF*/
                "Y50u0wHLpztrQbXnNFcznvY5RfmEatkODaf1beTizkUc7K/p4S4U3t1y3E8cEtZ4", /*C0*/
                "dws5U/YKs3u2e49/K085LYk0rxPCo3cChNBW3XTm9n1RJMRzMM1DVhjcodGLX7H6", /*C1*/
                "iw0V9atTtEKM4YgRnBA/kfk7tB9jt3vCGYQQ74Xm3evqM23Fh3zWdoTwQK/Ac+yV", /*C2*/
                "+wv1oVk95LwARtgEyidwuktkvqKY22/7Su09mX533tU6D3HfHp+j/EGIjYU8ieWl", /*C3*/
                "/Kyu5nbHisbrBmGGx2eAU+P9Ti81SnvSeOMBv7OifYRYbZI2weN5PBAWqiaHvFf5", /*C4*/
                "EsbHA0IsSZX8IYPusQ3WDUIr9gKp69nh8qQ3Dn2/LepI2vWLCbkf4TijjoE/If7b", /*C5*/
                "3VZHbTEhyxdqFECBSVKJuIhChID1EoFkNY/V98OtR0l3U2riKM4cZZv4N/PaAzgL", /*C6*/
                "47bm3OIWq77fcpHjyOw5jw6c8j9VyXxgpwtPTxUV/UvkjEC9tjBi7plOFg0RVLY1", /*C7*/
                "1BzB0hv/3a2gGm66LCYaKkZaRwkdKfILMt70raugk0xjPjMnE47ucyg70fUxIK7z", /*C8*/
                "XKXZsqt8bCT24B4sKMfpUNfToGWVoL0v66QEnGdSZRmC3aYp2mZ8E79W2ktra6cI", /*C9*/
                "EIbi45vFo3xI4nofz3XdajVkenNQdHQBO5K0spwl8qEQcs6OXvXI4Ju2UPOYQ5bX", /*CA*/
                "AVGC0U6GReDFoeGTkZZUrMeUB02TWtgjE0JKc8uI+rT51aO91r9cfCWIdDjpf2Bm", /*CB*/
                "gdSAodJv87r3mJUbrEDE6mywt/+ATaKaY/FYc1EfYHk7totBMRgH0QM6fa6mCZaB", /*CC*/
                "UeW6zdhSkT4W0D0gP3mthpGo1Ip36zq+GGZr100lECgnwzZm481dm4zFi4ZlrYoK", /*CD*/
                "KhWTDXhyBm2RgIJ7JAXn7j9GN/F2ub2ev8HQonM+3qfHagSgC6Bx+IY08gOolkxI", /*CE*/
                "+78Gu9q8xw+zCWM9+z9c92KREAPRBaPsYf9PIDtQPzeiP5ZUnywzzrTNQiKlIESa", /*CF*/
                "wZjReRFIHcdpJenORShaULVVZ932vznzRZLojSvV6osryg2SZW5oSzeK5j6kP1mZ", /*D0*/
                "ARXmF9doCtmmYZGhriC5m+SxDtmWbz0vfT1RELrsFd8xGPC0OdXl5jbO/ih1etHH", /*D1*/
                "r/vrtL7wT6DKo9o9Wwrp5UWh9AkiVFOEB6e1K9f/poBqvxpacMTgFH1nisEf7JnW", /*D2*/
                "dRUY4yShIXBflK0XKK563FpWDX8eUrWL7gUIL3nTcWYaOkGUo46I08z5W7VHfk3O", /*D3*/
                "aN6qPCZA9CJ7rC53h7J75+lAKpZ1N0kpG7mMpn+iBh7Hub0qkwLv3TNSF9yIO/B5", /*D4*/
                "kSM+tRfV9mU5HAvwomXbl1VIqoWbrncdcQebPps7ExnUs30a8dl2Y4Rh4Vov0JqU", /*D5*/
                "O5Kd4M00RP5ScDtx/fywAzhyptSQMnEtlu4p9m+UVisoxLOyAV2hYKAzOXmvXh0L", /*D6*/
                "S0kNDvv8QLze0bPHSEYco43sB5ohPBHxHL3CpbG+eCOUoXCAR8MMGSlj9czhttpj", /*D7*/
                "kb+6gp5rG5ABENZFTCMqh2xtzMGVaqRslSLw4BWTdyKs5ms0lpOlYEaISMsDpWf7", /*D8*/
                "C2aXTPdA1FAM/u1kv8oe5yLOGFeJGyCABtM1TYCJXaCG0LCiAgRwj8DfpEwzy28y", /*D9*/
                "1c0FoC7Q+7wVMVV1Rloi8GKvp5I2i+EICu2MDIJK0wXqgD7DzteoUZyHrj9QB8AK", /*DA*/
                "GlI8yLXvPvSsQYjEX7Dp5fm1hb84LMV5qfc7rhgIzSZ0X4/KMlw4T1JHlXsQInvx", /*DB*/
                "sKSYkZb8SBocTgy7QImn2ZF2DuabHXhCMIoPaPPK0Ycpr2pgk/Gf0JOHBGC82hhF", /*DC*/
                "NKBWShNIXfC5adyR616miWI85mA59MePkl5B+Z9rHPo1eTRWTp0hfoXvCt7WSGor", /*DD*/
                "/V8ES8X5fxqyaJQuVqmgRJK/Hi2iJRTxDo7SCxIzjSBi4YCvl0HX8pNs5t9zyu3O", /*DE*/
                "pMkpnUOQcdWdr14r3AfARNzOgKdI323vsSRgqeQFsmFd/sOEzraFGCHO4raGabW4", /*DF*/
                "mX4jNPqsbfPQ/rxTUutFvsAasrhGaSxAZ83p5/ymYCT8KSYWW22a+B6C23Sjzmz2", /*E0*/
                "HRrD3HRg4BbDOPrDegFy8WDthLdLzsG7ceYq6Lyoidq7wCaN+8/aptb2CZaugVEY", /*E1*/
                "YXbRlN3bEPGT82m6TLuf9QBePD/7YDtGrHiO/pa4YBy2Wt6VZ+KzXhliTWXUKHOZ", /*E2*/
                "YUviqxiRBiFq/M6PxLgcgQL5VvssAztIV8CMGUjoMvtwT6BiVIEgpuJZSYnROACQ", /*E3*/
                "0Oz/m3gJ1VqgnPYlg17N3YKMm80aj00VrzzOlF73fay+VIez0VRlbeeGhHOvZNu1", /*E4*/
                "v0OarKEIozWS5OazWoBHJo9YYW9hSrdLTvXbW+4CRvrbdHEKR2LdP6z6CKg8Q5VX", /*E5*/
                "BaZMd7iUoFEl8u50TJcHX1Opga33/ioC1m8zukvEZwk9uuyKkSejigo/PFc2hAv3", /*E6*/
                "w2YHQoNPN8988D6hARnzcQ/c+d7HVq1jMaL1Wei1W55MPom5O0W/0GCXccJmS+Gh", /*E7*/
                "SD79c8xhGLThS7A4oXkdRheo+zd90xBApCUyaaoiln2aeSaBS0UEIrZuzdyH1KUz", /*E8*/
                "IDMFejHQkdJ2Hw3l8Xh24sefFWM6Zj2aUrS6Zz9Ne60czjMrXsCcJsO8NNccJrgP", /*E9*/
                "7iVKPW1U0PPyt8korXQotCgtlimLeOxzU4UrE0g650pmC5Gg15lsdVxVLXHC4qfs", /*EA*/
                "3R+RJL+ln1VLVXA33vXdoUa5ZpMT/cPHpCsdiAzMfm6HS+Unam3nFHi4+sT71VJz", /*EB*/
                "CqyN7StimvEuWvx+XY5UNbJX8LR0ScjKrjUDPdB+Mas4F8XyOMEPcRY0QFpJzJCc", /*EC*/
                "YQTxNrnep5+dL0ZYC5tSoWXsucgRkRCf6jNORCjroyM3iRF1yoRAVcLqRsmnWJ1Z", /*ED*/
                "TqxWeLuqw26vzMvBB2kOmPXLAMetV4Uf9NlIr+Acy32Kq5mB39qzfxLz2/q4bbPm", /*EE*/
                "YBYQej8ND8CXxrYtsZRbYDhftqJ7T5R5vOJH3d6KhUArsJ8O20sEHjmVEKMBnceG", /*EF*/
                "KkiYWz2JMZcmA3n5Z+KN+11QOhQH9ICH46jTzPyuHKT+p7fXs/BCoKyhGbv+UTE+", /*F0*/
                "UGk6AHn27ecSq8ao1Jf8uppVeZy0lsATGVBLQwElBcMJ+63nI/uAEsAvvTFdkFkZ", /*F1*/
                "W8b8/EYMXFcZtDQ/h15i/qO4UX/M/ZYWwOdaDvNWnMcVrX/SHEbjFFkoR8u5zHsb", /*F2*/
                "Fo/M5Vj+Bji/wTRn104k+FbbXQ8OlAROe4sVxN2v0FvJxRGZlLrGpCaXrlXBKm7/", /*F3*/
                "REERuETIx0kAyzXKG+RQtHz5vmfsUCQwiw0e20kgZqj6uI5SIsBp2veiwYVl8btq", /*F4*/
                "TgR/9g1CuzezB+/i8+nA3EF4dxiGCBBNrtoFkzflkgSPEuQ3HHmkPRfqpMtFhGiA", /*F5*/
                "nY3P5HyKOVGlnKaR2PylSaUtOcIOpnI9OVWrebiWgg7eC9wurwDLa5A+S+VIOLm+", /*F6*/
                "GS6SGCrF13r1uPXEVywpKQvBd6WYH8/wzWC2OymfKwf7tnVVT2C4Cdv8uYFqOcve", /*F7*/
                "3iDCtK3XSiUNB6CVD+JibNibMDnzieutnyiLF8yF9qKlle5dW5BDk4Z8l6I0D8Pg", /*F8*/
                "qr/P9xrVnGYdJ4Bob4Dg45C/mOBZwjRpSXLK+/Dq/l22GEGLlEv3icMHaPvqCmRT", /*F9*/
                "IOFLwNysO91RjhlkvUlJbXSKWWaWs/0xgVrBwOltvLt5YsBhHCB/8t1cTujjrzgn", /*FA*/
                "3hFkWtMf180/Uoc+eRLAPPM2yg9y+M+wEvAAc+Bf6JMLCT8mhaCU6OyQ+toBCvYt", /*FB*/
                "3fq07rENpZ2aeXSZnOQk5bnLJGry/wnezees8+lxna+d7GYlprFi61M1fSGxKtAG", /*FC*/
                "IjXgGjnmMd0V6TVbBtGskxiS5wPMVuv08ZcMhVyBFwdvClTAimpPv/B/UOldrvj8", /*FD*/
                "B7JQ3BO0RavV2FGVNhe0QBByjhBqKDLKr5PQ2FReIgPpdqUeWaVsS2LgxE13qxJO", /*FE*/
                "bLls31D49NldM2cWG9i6iuVynOfPeUDLlmkxPnYvVOXEl9O5eUs+MBFRlIsDM2nk", /*FF*/
                "AiUNO8VcsXVYQ1Kga8ob2n5rwFZ1l9qeJRMKz35yV2z4w+V4Wj1h3FqUtKOULQbO"  /*00*/
            );

            public static readonly Table<byte> MIX = CreateTable<byte>
            (
                "BQMEAQIA", /*00*/
                "AgUABAMB", /*01*/
                "BAMBAgUA", /*02*/
                "AwUEAAIB", /*03*/
                "AgMFAQAE", /*04*/
                "BAADBQIB", /*05*/
                "AwIFAQAE", /*06*/
                "BAUDAgEA", /*07*/
                "AwQBBQAC", /*08*/
                "AQIEAAUD", /*09*/
                "BQQDAQAC", /*0A*/
                "BAIBBQMA", /*0B*/
                "BQMEAQAC", /*0C*/
                "BAABBQID", /*0D*/
                "AgUDBAAB", /*0E*/
                "BQIBAAME", /*0F*/
                "BAUDAQAC", /*10*/
                "AQAEAgUD", /*11*/
                "AgUBBAMA", /*12*/
                "BAIAAQUD", /*13*/
                "AQMFAgAE", /*14*/
                "BAABBQMC", /*15*/
                "AQUEAgAD", /*16*/
                "AwQFAAEC", /*17*/
                "AQIABQME", /*18*/
                "BAUDAgAB", /*19*/
                "BQMABAEC", /*1A*/
                "AgUEAQAD", /*1B*/
                "BQADBAIB", /*1C*/
                "BAMAAQUC", /*1D*/
                "AQIEBQMA", /*1E*/
                "BAADAgUB", /*1F*/
                "AgMABQEE", /*20*/
                "AwQBAAUC", /*21*/
                "BQADAQIE", /*22*/
                "BAMAAgUB", /*23*/
                "BQIDAQAE", /*24*/
                "AgQFAAED", /*25*/
                "BAMABQIB", /*26*/
                "BQIBBAAD", /*27*/
                "AwUEAQIA", /*28*/
                "AgMFAAEE", /*29*/
                "BAUAAQID", /*2A*/
                "BQMBAgAE", /*2B*/
                "AgUABAED", /*2C*/
                "BQIEAQMA", /*2D*/
                "BAUDAAEC", /*2E*/
                "BQABAgME", /*2F*/
                "AwUEAQAC", /*30*/
                "AQAFAgME", /*31*/
                "BQQDAAEC", /*32*/
                "AwAEBQIB", /*33*/
                "BQIABAED", /*34*/
                "AQQFAAMC", /*35*/
                "AwABBQIE", /*36*/
                "AQUEAAMC", /*37*/
                "AwIBBQAE", /*38*/
                "AgAFBAED", /*39*/
                "BAIDAQUA", /*3A*/
                "AwUBBAAC", /*3B*/
                "BQAEAgED", /*3C*/
                "BAMFAAIB", /*3D*/
                "BQIBBAMA", /*3E*/
                "AQUDAgAE", /*3F*/
                "BQABBAMC", /*40*/
                "AgUDAQAE", /*41*/
                "AwQFAgEA", /*42*/
                "AQUEAAID", /*43*/
                "AgQBBQMA", /*44*/
                "BAAFAQID", /*45*/
                "AwUAAgEE", /*46*/
                "BQAEAQID", /*47*/
                "AwIABQEE", /*48*/
                "AgMBBAUA", /*49*/
                "AQQDBQAC", /*4A*/
                "AwUAAQIE", /*4B*/
                "AgADBAUB", /*4C*/
                "BQQBAgAD", /*4D*/
                "AwAFAQIE", /*4E*/
                "AQMEAgUA", /*4F*/
                "AwQFAAIB", /*50*/
                "BAIABQED", /*51*/
                "AQUDBAAC", /*52*/
                "AwIAAQUE", /*53*/
                "BAMFAAEC", /*54*/
                "BQIDBAAB", /*55*/
                "AwAFAgEE", /*56*/
                "AQMEAAUC", /*57*/
                "AgQDBQEA", /*58*/
                "BQMABAIB", /*59*/
                "BAIDBQEA", /*5A*/
                "BQQAAQMC", /*5B*/
                "AwUBAAIE", /*5C*/
                "BAMFAgAB", /*5D*/
                "AwABBAUC", /*5E*/
                "BQMEAgEA", /*5F*/
                "BAUBAAID", /*60*/
                "AQMAAgUE", /*61*/
                "AgUEAQMA", /*62*/
                "BQQDAgAB", /*63*/
                "AQAEBQMC", /*64*/
                "AgQFAQAD", /*65*/
                "AwIBBAUA", /*66*/
                "AgUAAQME", /*67*/
                "AwIFBAAB", /*68*/
                "AQUDAAIE", /*69*/
                "AgAEAQUD", /*6A*/
                "AQIDBQAE", /*6B*/
                "BAUAAgED", /*6C*/
                "AgQFAQMA", /*6D*/
                "BAMBBQAC", /*6E*/
                "BQIABAMB", /*6F*/
                "AgMEBQEA", /*70*/
                "BAADAQUC", /*71*/
                "AQMFAAIE", /*72*/
                "AgAEBQMB", /*73*/
                "BAMFAgEA", /*74*/
                "AQQDAAUC", /*75*/
                "BQABBAID", /*76*/
                "AgMEBQAB", /*77*/
                "BQQDAgEA", /*78*/
                "BAMFAQAC", /*79*/
                "BQQAAgMB", /*7A*/
                "AQAEBQID", /*7B*/
                "AgUDAAEE", /*7C*/
                "AQQAAgUD", /*7D*/
                "BQIDBAEA", /*7E*/
                "BAUAAQMC", /*7F*/
                "AQIDBAUA", /*80*/
                "BQAEAgMB", /*81*/
                "AQIDAAUE", /*82*/
                "BAAFAQMC", /*83*/
                "AgQDBQAB", /*84*/
                "BQIAAQME", /*85*/
                "AgMFBAEA", /*86*/
                "BAUAAgMB", /*87*/
                "AgQDAQUA", /*88*/
                "AwIFAAEE", /*89*/
                "AQUABAMC", /*8A*/
                "BAMBBQIA", /*8B*/
                "AwUEAgAB", /*8C*/
                "AgQAAQUD", /*8D*/
                "AwUEAAEC", /*8E*/
                "AQQFAgAD", /*8F*/
                "BAUBAAMC", /*90*/
                "AgAEBQED", /*91*/
                "BAMFAQIA", /*92*/
                "AgABBAUD", /*93*/
                "AwQFAgAB", /*94*/
                "AgUDBAEA", /*95*/
                "BQMEAgAB", /*96*/
                "AgUBAAME", /*97*/
                "BAMABQEC", /*98*/
                "AQAFBAID", /*99*/
                "AwQBAgUA", /*9A*/
                "BQADBAEC", /*9B*/
                "AwUBAgAE", /*9C*/
                "BAIABQMB", /*9D*/
                "AwAFBAEC", /*9E*/
                "BAUBAgMA", /*9F*/
                "AgADBQEE", /*A0*/
                "AQMABAUC", /*A1*/
                "AgAFAQME", /*A2*/
                "BAIBAAUD", /*A3*/
                "BQMAAgEE", /*A4*/
                "AgQFAAMB", /*A5*/
                "AQADBAUC", /*A6*/
                "BQMBAAIE", /*A7*/
                "AQUEAgMA", /*A8*/
                "AgMAAQUE", /*A9*/
                "BAIBBQAD", /*AA*/
                "AwAFBAIB", /*AB*/
                "BQQBAAMC", /*AC*/
                "AgMFBAAB", /*AD*/
                "AwIEBQEA", /*AE*/
                "AgAFBAMB", /*AF*/
                "BQIDAAEE", /*B0*/
                "AwQFAQAC", /*B1*/
                "AgABBQME", /*B2*/
                "AwUEAgEA", /*B3*/
                "AQADBQIE", /*B4*/
                "AgUEAAED", /*B5*/
                "BQQBAgMA", /*B6*/
                "BAIFAAED", /*B7*/
                "AwUBBAIA", /*B8*/
                "BQAEAQMC", /*B9*/
                "BAIDBQAB", /*BA*/
                "AgMBAAUE", /*BB*/
                "AQUDBAIA", /*BC*/
                "AgQBBQAD", /*BD*/
                "AwIEAQUA", /*BE*/
                "BQQBAAID", /*BF*/
                "AgADAQUE", /*C0*/
                "BQQAAgED", /*C1*/
                "BAIFAAMB", /*C2*/
                "BQMAAQIE", /*C3*/
                "AgUBBAAD", /*C4*/
                "BAAFAgMB", /*C5*/
                "AwQBBQIA", /*C6*/
                "BAAFAgED", /*C7*/
                "AwIEAAUB", /*C8*/
                "AQQFAgMA", /*C9*/
                "AwAEBQEC", /*CA*/
                "BQMBBAIA", /*CB*/
                "AwQAAQUC", /*CC*/
                "AQMEBQIA", /*CD*/
                "BQADAgEE", /*CE*/
                "AQIEBQAD", /*CF*/
                "AgMABAUB", /*D0*/
                "BQIEAAED", /*D1*/
                "AgMBBQAE", /*D2*/
                "AQUABAID", /*D3*/
                "AgQDAAUB", /*D4*/
                "AQMABQIE", /*D5*/
                "AwAEAgUB", /*D6*/
                "AQMFBAAC", /*D7*/
                "BAUDAQIA", /*D8*/
                "AgMEAAUB", /*D9*/
                "AwIFBAEA", /*DA*/
                "BAMBAAUC", /*DB*/
                "AQIFBAMA", /*DC*/
                "AwQAAgUB", /*DD*/
                "AQMFBAIA", /*DE*/
                "AgQBAAUD", /*DF*/
                "AwUABAEC", /*E0*/
                "BQMEAAIB", /*E1*/
                "AwQABQEC", /*E2*/
                "BAUBAgAD", /*E3*/
                "AwQABQIB", /*E4*/
                "AQAFBAMC", /*E5*/
                "AwIEBQAB", /*E6*/
                "AQQFAAID", /*E7*/
                "BAADBQEC", /*E8*/
                "AwIABAUB", /*E9*/
                "BQMEAAEC", /*EA*/
                "BAIFAQAD", /*EB*/
                "AQUAAgME", /*EC*/
                "BAIDAAUB", /*ED*/
                "AQQABQID", /*EE*/
                "BQMBBAAC", /*EF*/
                "AQQDBQIA", /*F0*/
                "BQIEAQAD", /*F1*/
                "AwABAgUE", /*F2*/
                "BQIEAAMB", /*F3*/
                "AgQABQED", /*F4*/
                "AQIFAAME", /*F5*/
                "BQQDAQIA", /*F6*/
                "AQIFBAAD", /*F7*/
                "AgQABQMB", /*F8*/
                "BAABAgUD", /*F9*/
                "AwQFAQIA", /*FA*/
                "AQADAgUE", /*FB*/
                "AwUABAIB", /*FC*/
                "AgMEAQUA", /*FD*/
                "BAUDAAIB", /*FE*/
                "AQQABQMC"  /*FF*/
            );

            public static readonly Table<uint> ADD = CreateTable<uint>
            (
                "jMGyEw3jMK9hrRBfpv/16KyjUnl26RXy", /*00*/
                "usvJp51XPL+zYiCMjf6qVQEYFfi0Axco", /*01*/
                "OMxp7qlUrHQ7SMiEeFCbBe4ZMz/RjCRL", /*02*/
                "ZAJ+7jWDeD1zUIbUj0F0G+D2WsZTWA2j", /*03*/
                "vWvIotNSqLc8YZdfehl1X+gFiNepMjUV", /*04*/
                "izHV8h79MFxtE3c43U5vxL4LpJdu7s+f", /*05*/
                "DAW42laUHGXgshByzyG26m4D8mDAz/nb", /*06*/
                "FF5X2ByqhPDAdh6cqWxvM/7LcYc4LRPC", /*07*/
                "oHejhtoh+b9Xa60WRQqIyN3Opq45qZv3", /*08*/
                "m3QIoIr7PCmSTIp+EfKEXoV0zzw+w1SO", /*09*/
                "bxd4mqvscniZEeZ+zfA59w2iK4ulObNy", /*0A*/
                "bJ+3+WNN9CdbpuXQfiwJBuZg68QQ1ASD", /*0B*/
                "2pKanNGM5vhKf6dth+sTIIWG1NVXpdfQ", /*0C*/
                "d4hgEv0QY1s9bSlBg11DV9zfBqlybRo6", /*0D*/
                "5scSUVHX9uulJjD9I52jCPFPE1SKLFvs", /*0E*/
                "5u5SL9NTqECB7Ys132Jrx09ueI53NvZ1", /*0F*/
                "ftKrVLgr7rfBbXUXuzvetkC1kqlK5KRm", /*10*/
                "C2+Mm7HOojwJPFi9UTZxFN6nearF0cMo", /*11*/
                "i/e0q9iU4dPjhQqqlrHHyQ9xhTgL45ID", /*12*/
                "kOoioTYkGseCXD8i/6SwjNFXm1udaGrx", /*13*/
                "DTv6epNd4HWhB32+mIXIicHw2/s+9kds", /*14*/
                "P7rS08/wxGF9EkufeqWv6h2TzgiFaezJ", /*15*/
                "oix5sCxb1jnd5C99X+XG9EuzJm/yFiw5", /*16*/
                "YL/vWVqtbwUwu2wgvmWu3K7cedbb5k7d", /*17*/
                "48vgP67BFj2q6BAufzxN7Ixrwj4zL5XB", /*18*/
                "L6oV1aXnACQ6/nFOCf6N152YzbPr4zGg", /*19*/
                "DJVLaHqbRB8m773VrWJl6/P1KCv5+T0K", /*1A*/
                "uRE47IfdU3+HsqkLr2rUszSQDdP57gqP", /*1B*/
                "aGc2dqHW+YFW24Yzvwuy2ZfiAVY2YmQw", /*1C*/
                "hFzaf4EIO5VyzEqs7lpWNdsPVNPdZCwH", /*1D*/
                "qqsZX/XlHsleta/1oZboK9UJxOEP+ul0", /*1E*/
                "n81y03xuZ1v4WvtMIn83r4yFMckNzkVU", /*1F*/
                "fyw7QkFWaVhl+PdTS/GP3RKwhzfWcJB2", /*20*/
                "fUHQ3DqBFPm/20E4vikbYAG5t7RJpsPc", /*21*/
                "653iO9Cg+e0P+t3yzCMvksM7VsRyLxLk", /*22*/
                "ZkaL6JC6zt09MXtbCtwPvtkxRF21zgVt", /*23*/
                "a2TyaxTvS55oqn2xlrgec/aWebTpmQY8", /*24*/
                "lOKIPgJy+WYEvR1wfm/PkdSlgbzrMrbV", /*25*/
                "1TIMX4+zGAGLd3J8yVHiRXam6Q6a7Yzh", /*26*/
                "FUbsDTTK+rybzzN6AkL8f5hsAzx/EbAv", /*27*/
                "ghjoS7nGK9FCGaz6k7clyFlQIz2ysA0M", /*28*/
                "4klAHWq4hCp+239TDrSB7D852n3Q16Ov", /*29*/
                "RG1rAwNM3ptlzF6AZ4a0L3m5TInPuXmL", /*2A*/
                "JOehJnvkQcBcBFLFzAL1XUd+t4Y98zfM", /*2B*/
                "FDdz+PfpSYDfKyYA2yTj1qTBhqKLmU70", /*2C*/
                "g624KvZe/nHo1gVJd63aFqwMdQTnFO4A", /*2D*/
                "4YLkyUVPsFRrY8YF0h5QoyMq+/nMvl+J", /*2E*/
                "xSQ9rcx+AdC4gajfUjzcSC2d5pcc0sXv", /*2F*/
                "RMhWzCbwW4d62/CKe5+qyNjhd3k4fFWW", /*30*/
                "0k9wUU4XUcT8xcAdMgRpPEk9Ee7hOy8c", /*31*/
                "0qR75tAqoRywCV5MJgrJjO9csWvPC6IE", /*32*/
                "Hoao80u2qKvBcx2nZlU/s7vCgNez8aQ1", /*33*/
                "DuXGOu43ggXi08wo6TjBZWJfFjZSE0a6", /*34*/
                "VKSQwUtKsTuLT6pur/hUxNkJ+yN6O4vM", /*35*/
                "qFV7mjk9f7kc95W95F92+L805GDjyENN", /*36*/
                "lKdJdORUwBcRRB4DodGKy1iurUkePcW7", /*37*/
                "izLURGcVnWAFjicdqRrdWmT9yJj/VLlW", /*38*/
                "VF6/IqbY+dpDIyNtwfOOSHZ3FPB0xoVB", /*39*/
                "o3C/vmmNwB4bn05AF5FdRIAutzzlhVZH", /*3A*/
                "rGQ+VkoB03H7HHRyJssu8hrEUUVD7CMA", /*3B*/
                "K93p45YZwzegZl7lDR8t0bWZJTQvSG/F", /*3C*/
                "4+mp8Y0tDnc2ogtLcUx9eW/pFFWGhpO9", /*3D*/
                "Wd9VKi2YGMwmjTsJCkf3eAOacU+ef0NK", /*3E*/
                "aSKrlzOiIqwXHT1dHjUGeTpYyT2SIet/", /*3F*/
                "peMReb9mvNMzb7lw92TDEz035GujHO6V", /*40*/
                "uRtLYml/1bHavtYjWXPEFL4bxlO6DGOi", /*41*/
                "V7YUhvETzTDCuv/cZmkaCeyZidFUw6Ou", /*42*/
                "ywC7zoZFwUTeKrqXb+HgWIiL1u/IQnd1", /*43*/
                "m2HWrc6ARMaSvhWaXAUsfv9P4+DdRLdT", /*44*/
                "uI5ZGB+OELkVyLWUUzD6eXcTHODy12d5", /*45*/
                "EL3/r/Ou6g0/JGYc8wFm3ZsGEiCEXARD", /*46*/
                "S8osVEKoeFi2MC8pfYK7qj0LI9gvCGIy", /*47*/
                "8N1k2aZRg03WDRT8coZrLZTTMgVBdGfK", /*48*/
                "YblYm/Sa+ZuHmIpHfN53StS3YAWHGKOx", /*49*/
                "zBU0PImBS6z6k2ew8sC5wBUbnUfYU6Mf", /*4A*/
                "etMHU+uEdo5iIznpOLoGbbY6hMHO3eyn", /*4B*/
                "P6yTqw1Ow7i+nEcXIv+5ObMyJcuTnha0", /*4C*/
                "h0d7bPNIUCkBGEAnsMouWTqQNo1aJcfN", /*4D*/
                "pBLkGx2DanJftwDryvw7F9uuL5V9gepD", /*4E*/
                "EADE+xfbC9GuID2u0n80IDZ28K9uzIal", /*4F*/
                "SA8AA8wycyytCgKwTbbsCWQouNvA95yK", /*50*/
                "ZF7QwH0dkhHQmveSxV4fTMmiirMcgxEF", /*51*/
                "OAkZjfvoOm+P9O3IVKts8qT657AhqsIW", /*52*/
                "WJycNcNAkYi3vHBeVvOsWPuyvcPV1Yio", /*53*/
                "77n2hGhBVVy3yo1KQMPtXfd7xUTbrb8x", /*54*/
                "IHcsxBvR2pRddPwpOUYfPPxtL+Uurt2X", /*55*/
                "whSLDP6A2bC2I1YTH9dyXMqNW1vAmUrb", /*56*/
                "P997XglfiNKP+EbPze+5HuBruzxEdXyQ", /*57*/
                "iNibvXJsgKYPtsDmzi1be2wOq+fCzSDi", /*58*/
                "e1dVjBU1AqXnDWPGCng8PczuUDbL607B", /*59*/
                "uWGWcCIEd67FcM+m/qI/swrxOuuRiyL0", /*5A*/
                "UqROJDDrgBXnR8HfLsFv+Zse9Rd9Dufg", /*5B*/
                "DtVPNLffhI96GTmFiin6L6Ui5VgbdkZr", /*5C*/
                "E3AEAPO2Y0Z5ThhYSTRfygjFB5ZMQfon", /*5D*/
                "w2jB7zN7AdyNpDF/bjxLa0JS0DwoNzxl", /*5E*/
                "ikTJqgm5vyWzPiFC9+BuC1IRMnDqtrVA", /*5F*/
                "40clT0iLyIRM1InHRn0x6I4xHvvYjTOj", /*60*/
                "PrcP2IzSRIHMQaQzVGNzt24qG6lHMZc8", /*61*/
                "Hf/C0IGrC/hZYwB+kAAFWrx0g5LpxTbm", /*62*/
                "Zm3JKWx84q5R4XwmeHN0QNnUTLDzwUBR", /*63*/
                "XndrFdA1TZo7X9qEek55Hi2VFWx31jS+", /*64*/
                "6/H7lURnyktsd/ByzbQRfDxOZ6WkLQKn", /*65*/
                "9phwbAOiNTWTTyJj8fblT8AK1k8x8IRi", /*66*/
                "fXXxtanhovB9JkZAyuZlI7eEHPD1d3Gi", /*67*/
                "X6Wo9yPS5Ciy2zNpfGVuwLeUTECOJ8nN", /*68*/
                "SduPuLAcPNuacUNmVxGKp0lWiXQm6jWb", /*69*/
                "pgBS4I+8jK81aTZ2cjZljJv5oxCqKS84", /*6A*/
                "ADmrKcREnChf5GMV3mUKqey/oDDO3J+r", /*6B*/
                "pgry4cppneykbiUDO7K/1weJg5p3Qq0s", /*6C*/
                "wzdr3RSJboFG2fVQkzssOc1iIz9B9/5b", /*6D*/
                "AYR1bZCLzNq7cQcpGKN57W9Shxp2Pi4v", /*6E*/
                "Fq2DM/pERw83exyKEyTMjlopMnevofaj", /*6F*/
                "Z1Oy8VvrPiOwUp0nymwEmgdl17CrY1mI", /*70*/
                "+eFz7HLXrm9yD3jsJg7FRlvbIWLmdcqJ", /*71*/
                "XZ/Y2ngm21kYwHml9QjakF55nr6I2eQR", /*72*/
                "H69zI63Zz8wVnl428R86dO18BRNiFtFt", /*73*/
                "q88q43ieZezycrQMfpAC3QI4zv9jGR+5", /*74*/
                "LRzMQ5mdI6RrMG53iJp09ZBk7Zzc8J0t", /*75*/
                "6TKKPX5V2/Px7tOnQWOcMnM8bTr4IdAJ", /*76*/
                "YnbedDrxsI8J0ZNnhmro7yZjOZp205bv", /*77*/
                "4h2SfNVVTMJb1EQ1otmGVjVW8l1U6cfv", /*78*/
                "HXhIbAaDjOoprXp1h1wRQD3ALqnCPm3z", /*79*/
                "AexIisbux2poEv/gxydRCh4HRALt0Lav", /*7A*/
                "M52ymcvNjV6o1C7afv3M2drki2bDWBgI", /*7B*/
                "B2Lc3cMpI7VyuR006DCrT00fgv+djLZH", /*7C*/
                "piGV1Q01hevGR/XMouBQ71qvSbWExLD7", /*7D*/
                "9OkXUhMjfLKvIivI0EaaYfr0D07V/rca", /*7E*/
                "pb8+kUAkVb6lJx64qQ34/mQIEFzujj+8", /*7F*/
                "E3ZiJ5HWU929oboZEeh50aD9G5I5xRo+", /*80*/
                "B8XKF+CPqWqF4AL7++7Q9vSZZ2dAnTUS", /*81*/
                "7ppNVUlSot4cULzEsrQR8NyEuVhaa5yS", /*82*/
                "yK0XMu5tmv0VHpLDwVnWq2k506tnZzCT", /*83*/
                "r4wyNaLXxPN+NLAH5Sf2PYzHc7LNCPKt", /*84*/
                "H3LXKwM4ePUklrsbBbG0Vc/zs/+z4fJM", /*85*/
                "aN1l0W0dwLFiHhAdYlYKOtIihzxa/0S3", /*86*/
                "GISbROvOxSnNA0SXq2CW5JlRFIJ7ts4g", /*87*/
                "c6QrfHmArufYLUMxqfWHh7UoQuAvLzkz", /*88*/
                "+hYnlhIdbwpY+LNGxxniYvEqHoYysmZY", /*89*/
                "UQKzVzca9WJoQTJZMG6D8AG8g8CusYt3", /*8A*/
                "nsihcUw6rOxkzrs8L4310QZuXGqVQcZh", /*8B*/
                "HKIFfNnqWxVc/91+mIy7eBFO6Kj5se7F", /*8C*/
                "s/+fQ0YTrfGxi4mcmNC0+mN47zDoRnaL", /*8D*/
                "fIG5B4mjqZY+hphG4VZDAhryrzgJfRH0", /*8E*/
                "cygLp0Uxikgxx6VD05+kEuUAAL0BkF6b", /*8F*/
                "D2PWF8wPrvPrT9A2qMS4ILmCdXF8jVNI", /*90*/
                "/kWwkZX23D9cDW01joC4LuNMTv12mskd", /*91*/
                "KufxREb0MEXWRVWyjUmAULLYxrj0uDLL", /*92*/
                "VywGqETdPj+M6e5IVLle4tu9Ehfh33BS", /*93*/
                "wgH5EpnZ5p1piXLF5V0sBuTFSTytSjRb", /*94*/
                "1KlBe73TFwDVRlqshCKvr6jO70U/xs4z", /*95*/
                "pKLqsySDkBUA/rXc1+nx0Lxi41GIt8v+", /*96*/
                "BebVL9rK/cbrnt/kF56jIslg7pSHTc7y", /*97*/
                "EHRiXsjqWN3UDf92+c8apZpdbkCIxPZB", /*98*/
                "+WAIkfviiIxKOz5v2fVjtULgDz9Rcrvz", /*99*/
                "mAhyxJmISxd+YifQ5VRex3APOv58aTq4", /*9A*/
                "+/2zl+LVgN7Tr02XM2hQg/B+qlGp6VEn", /*9B*/
                "rrBqSEdKxzmeBeWLH9IG04clbEYYW8Ow", /*9C*/
                "TErUU91OXE0rAkfvdZj0A4Rn6lim9Lgd", /*9D*/
                "Y2xt82TOPyh4dicfcgtN2tYH7E4Rle2L", /*9E*/
                "GdAlUT2c3zGoOY9VhTmCLtljDzYvuP5h", /*9F*/
                "uEF85PKtDqe0lFruGGepmlv4ekYdB6JG", /*A0*/
                "FBAS7iah9y6FL7FjMPBSqmlaz4zQxfQu", /*A1*/
                "dQLjVi3JPpymq8AD6m2BEJ7lq3VVAwap", /*A2*/
                "hJsxjFq1/81EoAKWjwWDQcbKUGy8K6F2", /*A3*/
                "RaElHLX1HgRgCkIHzp2nMFGYxJqEmFC6", /*A4*/
                "1NR2wB5z6NVnwfGuaU3OUsJDEkboSxJZ", /*A5*/
                "x7J50ZgU7gbQZe+Gj3ZHxFO/9IfFAtAA", /*A6*/
                "p7+3+5b+54N3Z4rAkjj0BYV/IbDDH8ks", /*A7*/
                "dSYD6NlAcKEQ0YupW210e1etqnHAxXDP", /*A8*/
                "tqUyB0eGqAiLhOpjYq4iiSR4NwDwQrbm", /*A9*/
                "WfEflCqobww3ttdVrhKl1TClpkdGfW2z", /*AA*/
                "gbRJyiaLePMV5cAW2feuLVunR2K1S0h4", /*AB*/
                "k04QZRcnYKoCze425Y4/hM6soSnVC8m9", /*AC*/
                "nSZiD8cKN2345xYay0g0RvtkuLgUqp7M", /*AD*/
                "ZN1aoIhPVNEOiY6A/gdDrRWjnduIkDwK", /*AE*/
                "Q5e02+LAHYLYLHrlx2/w6XAQPjbVqVop", /*AF*/
                "RLQmozdGH2IBaGGSu18f/az+b9yAHLFY", /*B0*/
                "x0kuIZXQUdF2ojZLZn3XKjesGs8geXv3", /*B1*/
                "gEKroiCpcTl88Aa5jLoT2cbRKQR8uAsH", /*B2*/
                "rfqrNFkkZo3JA12cE9eGs+N8c60T+fly", /*B3*/
                "t3iNLtBTBKNxQ8X0gguO0ZUZLSt4rDMD", /*B4*/
                "BP7m4V2nBJjeER/72bYg0pG8tHxhaPdu", /*B5*/
                "WqWHvuaX1wDRgWIPvDUiJE8FpPfbu1fq", /*B6*/
                "SQB9mNiYwAPVZsnAAnKL1qBqxw5ZZkrO", /*B7*/
                "0xQXcDpNK8yDWo7KTN/kGni37Vn5rMAz", /*B8*/
                "IgEL6ZMmAOhEOSsVQCX0IpREV8bijMJG", /*B9*/
                "rIKxcvb3Aalb1P1ceXmpOY1d58FBWjx7", /*BA*/
                "tHGcY/jPD74E8WxSKeP6VLuYIAN3V7Xn", /*BB*/
                "m3ar/kkHbClv8DumfGItDCV0YIRpQPmk", /*BC*/
                "pzjJ8kQe1CdZqlZ+bG7kbW387dsJFEAE", /*BD*/
                "AYj8MmDSFoX+GtZkMkVZlhLKuh5rPAlv", /*BE*/
                "rL3NTZll5k28qpQnnS//kxqhKn5Kh8/w", /*BF*/
                "a2oW3nHtyLpoGv4235bwwMVBt4VIlbuW", /*C0*/
                "0vLa8RQh0ouRn5Dm72B+dvL690xXGoC7", /*C1*/
                "W6C6Rnpt3SC1dGdL7d2dnUIB1v0KpVFT", /*C2*/
                "udJUEcijQ2I8GrrvU9aA2FZSzVmsY+9L", /*C3*/
                "yg9/u+49z4vo03zvpW6aQBEqW9B9iz/L", /*C4*/
                "hgLfXCsyqhUZRfeCNSV2GQDqrhATu8am", /*C5*/
                "ZM+MT6L5dcGIIIpFC1ZOMzFK2T6g/m/D", /*C6*/
                "KZB0KhbpgKcGNxgyjlIvk+TDUB0Y/Ing", /*C7*/
                "j3Imqm0IncJcjvhhjervLN3xiO4PupvI", /*C8*/
                "V83ipbfEnWJ5YHLHsUCk7nR2fVB/7T6g", /*C9*/
                "mDC3I8eW6XvavYtsNxqIfTd7RBJAnS4R", /*CA*/
                "kgodwgoBmtDzejXNP7YmBSbf4tE0gvRP", /*CB*/
                "zVkNleHbOQaDJzOcRsdJ+uvHPZXkNrGO", /*CC*/
                "/vSomYYWaomD5RveQqbjINXlZQ7M2qwP", /*CD*/
                "fd2KT/99esJhhkNlsmu+BIyVgN8ymjZn", /*CE*/
                "zpJOECCf4ZTzhqpvjeQlmk4gggQbLSyR", /*CF*/
                "JIisKL883YEKOwDUMjaoXwpeSF+KmoFf", /*D0*/
                "q+bQ5sh96Qm+bZRecwXZfYan7PW80Luw", /*D1*/
                "8XfTuDe2oo8KGDUrYkFGKDXQVcQhkPp9", /*D2*/
                "BynFasbkSSfQpNZq2nRxO+/jGf4K9PH5", /*D3*/
                "RQMQ2/7/1fVGRa9axWxaqTHicrFf5LiS", /*D4*/
                "Wyx9YQy7M+FvfM6V8btZ6Pd2JeFhitJ0", /*D5*/
                "TsqEwt7c88zb+ZzG06WX09ycgAI/h7x/", /*D6*/
                "3FhxF2rcMCvYa1Kvy4PnecU11shBmrho", /*D7*/
                "49sPNRynmpDzxpnBomYFl9o3v1hWB8pi", /*D8*/
                "cZFii9kLxoOd3udVif2HCpFaFmOFXL7E", /*D9*/
                "4gKLe8yotZizEr6XyHzB74ARDmLSuea7", /*DA*/
                "kGuTl9eoWTCZLCSZW3ESUqLkQDv/MLNH", /*DB*/
                "SC/o3qViWJxSgclGwcAm3uLcfKAM8mm8", /*DC*/
                "QTHqqko1QPiVoOF+rG2PSW0h6Oqgll62", /*DD*/
                "nF3XEjEVqmp23yCugXkl/nvnTkN5dZxU", /*DE*/
                "aMQvGwjhQAmeGt5giW/UQRLWDc9rOKB6", /*DF*/
                "9jNkpboAkXjpt81QbK4ROho2JIIUPIsf", /*E0*/
                "HDpt/TKmLekUWrMIzSH5bVB4iL6k0hru", /*E1*/
                "9FksdyEOV7kc4Eh0HrYypuQYAkprk9RI", /*E2*/
                "xLccANcRJWSpVxReUxzXGzC7jvgSn/eq", /*E3*/
                "nJkpO3uo8mGEjBynS9c/CzDCGpmcGNUc", /*E4*/
                "Bdvwl8vbZkFbCM35afU/CL3Olc0srKSg", /*E5*/
                "2f4ojDlnpwsDHsjXaZPsKHmzfvQkxgNO", /*E6*/
                "rSfviDYSlxc4zOhNa5Jo1LOS44uRQDlW", /*E7*/
                "DGt7rThtCOY+EMx0q8RPc4MzlhgJ/df1", /*E8*/
                "dQm0NhmSQX4j2wnRb59W3fLzXcysKnxj", /*E9*/
                "/+BUw+OrHXHq7gg4eWYgbOpAUNdBbPtA", /*EA*/
                "Rqua3WdO5kFbQhomfD/LgDmhMaX6TZN1", /*EB*/
                "CErkFIuijB9Clcb/aRpRs0wDqvgBLuUm", /*EC*/
                "iSBbXA8LfLMYvdDYLyyTQO8v7txTmYJS", /*ED*/
                "b1bqN9FNnAhi12CfhlR48g1rOqk9BgNl", /*EE*/
                "x+LLwCbFI07wcmtgBXpLiHv2bqYK2A3z", /*EF*/
                "4Hby85LGkTVbiLuyWC7C2ynKbXgGDeoV", /*F0*/
                "lqK3kJ3swSzmKpIg344LNcG2DVglZx6Q", /*F1*/
                "Id6ZwE5Z4R1RM9JI6B/O0XFm4nZKdF6A", /*F2*/
                "Ex4HiXqk6Oi5J+cs9rv6vlandD8tPuJa", /*F3*/
                "3OUI/YzkZouyJc1J59drNbrpKpNCTc/A", /*F4*/
                "PgdhrcAQbRTGhLkVpD/rB/GlvK+/g6EK", /*F5*/
                "pEch+HyJPd9KVTuqgtJq40vhta3KhEVV", /*F6*/
                "tGfshCY0S9jJpvAr7/iji4taLdbKZdkX", /*F7*/
                "DKu1A/Eqaiizn7U98YuP/UmLvmge0gJS", /*F8*/
                "fDBMIYDRZdShcIDEVXhnlLcYZ1yzT2Z9", /*F9*/
                "+N+tC8xOzEzMbzHhVVgKyvSMDR7Y4x4M", /*FA*/
                "hyFq89tkXpzi8FpMj0VFioHTBIJ0GONM", /*FB*/
                "pa92Tw1yY/+M/gkfAunXiNhMwG+X6ylV", /*FC*/
                "YkmEYyKd8IViUOIQT+gLnLVAlAMyur6P", /*FD*/
                "J6RHxOuI3LpyhcQ+POhMKX3K5EP+3K/V", /*FE*/
                "TyZ9zH5NddtcVyANgbrSDarYbosjUYau", /*FF*/
                "RFJqYyG1PCD/M/kG3EibuTnkZbdWIqiL"  /*00*/
            );

            public static readonly Row<byte> FIN = CreateRow<byte>
            (
                "GRIjFgEfCQAMKgURBxMNAhQuCiYrHCAXLQQpGiEGIigPHiUQCy8bDhUsHScIAyQY"
            );
        }

        //=======================================================================
        // PUBLIC METHODS
        //=======================================================================

        public MHash384()
        {
            Reset(); /*initialize*/
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void Update(byte value)
        {
            if(finished)
            {
                throw new InvalidOperationException("Hash computation finished. Must call reset() first!");
            }
            Iterate(hash, Tables.XOR[value], Tables.MIX[rnd++], Tables.ADD[value]);
        }

        public void Update(byte[] data)
        {
            if(object.ReferenceEquals(data, null))
            {
                throw new ArgumentNullException("Input array must not be null!");
            }
            foreach(byte value in data)
            {
                Update(value);
            }
        }

        public void Update(byte[] data, int len)
        {
            if (object.ReferenceEquals(data, null))
            {
                throw new ArgumentNullException("Input array must not be null!");
            }
            for (int i = 0; i < len; ++i)
            {
                Update(data[i]);
            }
        }

        public void Update(byte[] data, int offset, int len)
        {
            if (object.ReferenceEquals(data, null))
            {
                throw new ArgumentNullException("Input array must not be null!");
            }
            int limit = checked(offset + len);
            for(int i = offset; i < limit; ++i)
            {
                Update(data[i]);
            }
        }

        public byte[] GetDigest()
        {
            if(finished)
            {
                throw new InvalidOperationException("Hash computation finished. Must call reset() first!");
            }
            byte[] digest = new byte[MHASH384_SIZE];
            Row<ulong> row_xor = Tables.XOR[256];
            Row<uint>  row_add = Tables.ADD[256];
            for(int i = 0; i < MHASH384_SIZE; ++i)
            {
                Iterate(hash, row_xor, Tables.MIX[rnd++], row_add);
                digest[i] = GetByte(hash, Tables.FIN[i]);
            }
            return digest;
        }

        public void Update(string str, Encoding enc = null)
        {
            if (object.ReferenceEquals(str, null))
            {
                throw new ArgumentNullException("Input string must not be null!");
            }
            Update((enc ?? Encoding.UTF8).GetBytes(str));
        }

        public void Reset()
        {
            rnd = 0;
            hash = Tables.INI.ToArray();
            finished = false;
        }

        //-----------------------------------------------------------------------
        // Convenience functions
        //-----------------------------------------------------------------------

        public static byte[] Compute(byte[] data)
        {
            MHash384 hash = new MHash384();
            hash.Update(data);
            return hash.GetDigest();
        }

        public static byte[] Compute(byte[] data, int len)
        {
            MHash384 hash = new MHash384();
            hash.Update(data, len);
            return hash.GetDigest();
        }

        public static byte[] Compute(byte[] data, int offset, int len)
        {
            MHash384 hash = new MHash384();
            hash.Update(data, offset, len);
            return hash.GetDigest();
        }

        public static byte[] Compute(string str, Encoding enc = null)
        {
            MHash384 hash = new MHash384();
            hash.Update(str, enc);
            return hash.GetDigest();
        }

        //=======================================================================
        // INTERNAL METHODS
        //=======================================================================

        /* Magic onst*/
        private const ulong KMUL = 0x9DDFEA08EB382D69;

        /* Hash state */
        private ulong[] hash;
        private byte rnd;
        private bool finished;

        /* Temporary */
        private readonly ulong[] temp = new ulong[MHASH384_WORDS];

        //-----------------------------------------------------------------------
        // Initialization
        //-----------------------------------------------------------------------

        static MHash384()
        {
            if (!BitConverter.IsLittleEndian)
            {
                throw new SystemException("BigEndian system is not currently supported!");
            }
        }

        //-----------------------------------------------------------------------
        // Utility functions
        //-----------------------------------------------------------------------

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private void Iterate(ulong[] hash, Row<ulong> xor, Row<byte> mix, Row<uint> add)
        {
            for(int i = 0; i < MHASH384_WORDS; ++i)
            {
                temp[i] = Mix128to64(hash[i] + add[i], hash[mix[i]]) ^ xor[i];
            }
            Array.Copy(temp, 0, hash, 0, MHASH384_WORDS);
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        private static byte GetByte(ulong[] hash, byte index)
        {
            return (byte) (hash[index / 8] >> ((index % 8) * 8));
        }

        private static ulong Mix128to64(ulong u, ulong v)
        {
            v = (v ^ u) * KMUL;
            v ^= (v >> 47);
            v = (v ^ u) * KMUL;
            v ^= (v >> 47);
            return (v * KMUL);
        }

        //-----------------------------------------------------------------------
        // Table initialization
        //-----------------------------------------------------------------------

        private static Table<T> CreateTable<T>(params string[] lines)
        {
            Row<T>[] table = new Row<T>[lines.Length];
            int row = 0;
            foreach (string line in lines)
            {
                table[row++] = CreateRow<T>(line);
            }
            return Table<T>.Wrap(table);
        }

        private static Row<T> CreateRow<T>(string line)
        {
            byte[] data = Convert.FromBase64String(line);
            switch (Type.GetTypeCode(typeof(T)))
            {
                case TypeCode.Byte:
                    return (Row<T>) (object) BuildRow(line, (byte[] src, int off) => src[off], 1);
                case TypeCode.UInt32:
                    return (Row<T>) (object) BuildRow(line, (byte[] src, int off) => BitConverter.ToUInt32(src, off), 4);
                case TypeCode.UInt64:
                    return (Row<T>) (object) BuildRow(line, (byte[] src, int off) => BitConverter.ToUInt64(src, off), 8);
                default:
                    throw new InvalidOperationException("Unsupported type!");
            }
        }

        private static Row<T> BuildRow<T>(string line, Func<byte[], int, T> convert, int pitch)
        {
            byte[] data = Convert.FromBase64String(line);
            if (data.Length % pitch != 0)
            {
                throw new ArgumentException("Length of input array is not a multiple of the specified pitch!");
            }
            T[] row_data = new T[data.Length / pitch];
            for (int col = 0, offset = 0; offset < data.Length; ++col, offset += pitch)
            {
                row_data[col] = convert(data, offset);
            }
            return Row<T>.Wrap(row_data);
        }
    }
}

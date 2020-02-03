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

package com.muldersoft.mhash384;

import java.io.IOException;
import java.io.InputStream;
import java.nio.Buffer;
import java.nio.ByteBuffer;
import java.nio.ByteOrder;
import java.nio.charset.Charset;
import java.nio.charset.StandardCharsets;
import java.util.ArrayList;
import java.util.Base64;
import java.util.Collections;
import java.util.List;
import java.util.function.Function;

/**
 * <p><b>MHash-384</b> - Simple fast portable secure hashing library</p>
 * <p>Copyright(c) 2016-2020 LoRd_MuldeR &lt;mulder2@gmx.de&gt;</p>
 *
 * @author LoRd_MuldeR &lt;mulder2@gmx.de&gt;
 * @version 2.0
 */
public class MHash384 {

	public static final int MHASH384_WORDS = 6;
	public static final int MHASH384_SIZE = MHASH384_WORDS * Long.BYTES;

	public static final short VERSION_MAJOR = 2;
	public static final short VERSION_MINOR = 0;
	public static final short VERSION_PATCH = 0;

	//=======================================================================
	// CONST TABLES
	//=======================================================================
	
	private static class Tables {
		static final List<Long> INI = createRow(Long.class, "0wijhYhqPyREc3ADLooZE9AxnykiOAmkiWxO7Jj6Lgh3E9A45iEoRWwM6TTPZlS+");
		static final List<Byte> FIN = createRow(Byte.class, "ExkVGwARCBosHhINHA4XJhQqHQMgLQEfBSQGEC4nCQcWKAsEIg8pGCEKLwwrJQIj");

		static final List<List<Long>> XOR = createTable(Long.class,
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
			"AiUNO8VcsXVYQ1Kga8ob2n5rwFZ1l9qeJRMKz35yV2z4w+V4Wj1h3FqUtKOULQbO"  /*ZZ*/
		);
		
		static final List<List<Long>> ADD = createTable(Long.class,
			"aFqWicJYgH2W7G4/yToOpIjaZGq1KpZCkao0WdKRY/vMza+Hfo+V2fj2yYLRFIOY", /*00*/
			"Rn1i1iQCRhwCo3do7TiDDGseLYqEhpYJSft7XSFts0MEfNuU+IaHs5CLtuCqOEZv", /*01*/
			"ziHHKliwc9aLYUO6zRzRBzmcf5g/m4/WUcN9NqWcxhvVVxQjtV8xGVH/xCOA/0m8", /*02*/
			"5C2jkTpkeWEDM43vHS6HGFLaElQx2L1/q6g/tcq1qPHGOsKK9OMdgEiqYYGc5Hqv", /*03*/
			"u7CbEYxqmwzhIB5d6FrlDUFry8ldQ7dIQWjvT+TipqNgALho6GkW+cvKWIKQz0sh", /*04*/
			"sn2U4h05lEW8wp9uzHFlPiE4Isi7egZkR+ACR4PxtL4Fvkah2QbxtENoS2H+6J2b", /*05*/
			"TjdM3NZbNwln4mDrkRazjERVNvAduBWpUTtIVLB6BJ9+nI5om8cf0IsRWSPep7jM", /*06*/
			"sN3svEANJ/dwfr1fxDv3xBB+UInv5Q/XyXtq0a1NpuPW+YyzVGEBzlNT2N/Gpgam", /*07*/
			"pAzgiORuufgBgfWu7lWJBszBYWrPWatLA6rpXi9U4D/8k4E4IoaQ9sV0d/Tupou3", /*08*/
			"rq0IIbJzj3mQBBO7RynXNwD8rDlMHj2ksWJHQTUdsc6kCgJg9RyN6MiRdBX5IgPi", /*09*/
			"vuF4sMtfYdoCrDXs4QnzKK7YhB27Wrdkbl7wWiDamjvvyMuP6I5rDUPJaRcBrYqv", /*0A*/
			"3AECGDjAURI3/r9XxwVjmlYZHM1DKZbbdJpWWrnClIqdBBagfOq0+wrwd0SPBAPz", /*0B*/
			"5hFpy4goNxZnb7ys1XnFMAWQ6gvQIBvEQAJN2smQGYhguKuNTyGZBcg1peMKYknk", /*0C*/
			"hB0L+eoTaRHh6LE0YRlOXlntdSUhQ49LT8N+zbaYdpw+8ms5VCrBlAkZ0QLYGf/p", /*0D*/
			"inYEWuiga4yQAlW28LM3jV2cPnN9CMLHmPLp4bgyuafHzOkBxCJROFpCJ/efgeuz", /*0E*/
			"bJ2wGvvbdV+XQR2sPAg3uGBydSsUia/ZSP7jHedxH6yQV4wJRBr3cV45GC0C5JMC", /*0F*/
			"Nj0AiLBh+8gcfwe1BBb+PMm7BmfdIixNQX0wu6lW0DmUOl+hKlOGLXoBzCqoH6+k", /*10*/
			"XN15K6F+aXQ3CQ0GL1ompnBdqO6I/o7t1QlOKKh8o5+XwEgwjkMdHeLaoWIziBml", /*11*/
			"iuSqukAIVmgH8we3Jxw40An4ItskTo86ZBovWG1S9axNkOw14+R8wHBJ3MN1+Pvi", /*12*/
			"72l0soJ5AVKkdvCiyJ96xjHpLOJ8FNf5L7N+R69TwS2QCJqIZWs54cQqqa56XU6H", /*13*/
			"mb1MzOG+F+Y5acq9qyvtrHG4l2LnUIVjIY8XXvY68KpQtMqB/RJ9W53R4byprHJn", /*14*/
			"euvvQbOycV4K7m/eBQusHlTAGNte9oZYgLMpx2ZUV4sKemqIIAIzzi/Qhhf/8t8o", /*15*/
			"721wssckJ4AA6Sp9FFL+Mxy6t6Nequ7IPkKPXSB/1LtGrwg6X87Qsgrzg8wVVlOt", /*16*/
			"QnWrYtl7s8ljna6Mjxg9F2D9doBJembz5JLh9/B05wi4bKEDePdPr9Yn883ZDOev", /*17*/
			"qhUeIEORodNQSlW1rrwlEECEr9x/drhZT4j7yVMzRZLQ6QE07UIn8yLFtYIOLjiM", /*18*/
			"enwLuQhZC88GZziuPMVx2mjLBcrt1pFP88Zc37jmh4bC5+Qbmw+Iow6R5LXhYD+w", /*19*/
			"xmkQeImzw/QoosL8FT5P9cA1BCh/iHv5wKhQTuzfg6UGwu0XtOV1lq/1fYAmaI9h", /*1A*/
			"WmEiLQ1sRoMH8gae4SBFizTgAiWeZYtNqengS6A9ZZ2oxgS69xJXB5gG1QZqZ+KD", /*1B*/
			"cvO478wzZfwR8JT/ZBDWAGFTMvr3OokJX4tRu1C9woicILLpZctomI6OErcn0ifz", /*1C*/
			"krpeyDMDX30r8jfOyUuksslkjzAaH7fWGY7Jn4o6oKmWjxSbZeuLsVLVnchzEP/n", /*1D*/
			"vHWLccdCxFMYakLiwUq06NcNcmrJug9acsL1UO1x6Vqk88iCxmvbu5ZgXMUotzzh", /*1E*/
			"LfbJnutwvVqDR7QYoT3vsIKWhuyq64LaM9WIQ2gdpIoEHq0QcWM1kowLLAhAiy+7", /*1F*/
			"6OUDeoZ7g5bHcVaAq2kmjK6XcVPaMifzAOsU8WKHkJXXKgi/Dfan/EhZ0oDnwn6j", /*20*/
			"GtaqBODyIX85e5yQxTZm2N/6cKHuFj5RYOvHdyu8JbfuHwGsiY18sQq/77cwKd/N", /*21*/
			"wVtvuY4zH8oSriCa5EQtBhC+cXsuu20VkInmS/nwOKDVXGiucIPmI4PpaXCyWjrG", /*22*/
			"p6GZYzpY9I8FfvYmvSY6p5DV9VKaSJFdSdlHULV3orMIvCE1dIkirhqQwfUwu9t1", /*23*/
			"E/566UIw4UeXo76UjUCfKtjOVhNJXs9vUFNkTwQj0HXAVI/k3WAtuCc0MUhmZwt+", /*24*/
			"4AegKrSLhWQhcGIMNWtMgMz5Hln94h9cQ5fuAhhyZkbkTVvQ9EQjERo9QmxxwCqv", /*25*/
			"1G/OdyHDgVpkejcsphnoAAPwaiBrKuK9gcMFWRQu+Of61q7pposXI2K6AKmEbLmx", /*26*/
			"mL5205MDI6cm8dYgIH7PRxsQTLwR8qJXcVulE/P3746PgIM9YFJYoZpxaBS+Zg+k", /*27*/
			"5/mhMmlsrJNzVheOkRkOkX2K3hp9Eq9YUa3GU/APunxexdapambf8qOz7Tb/uj98", /*28*/
			"GMWCYMYqsUkyc8HO3NUnvWkYAj3CGpXciwdkTPbHNvu9RptXMQNZYggjgM4GGlvk", /*29*/
			"SRmwqkpgw7J6qGxnAimjtsnqOpjYpqGLyMnFh+yx9T5MUhtZkZsrYwpY7eHVr673", /*2A*/
			"+/GygIMqONASzxQttR+gn1G3WJApuSQiGPtUjRjrYB+g1ooU/ijgVni1VLdyzG2n", /*2B*/
			"CWmqAosQExttzN5uKR49ncFo4PyHCqslSFc6USgikUXWJ4OqhUGhFonvRU27kiO0", /*2C*/
			"PfAPkkRz16FF+bunpT3UDkb25PlRYsxEiovEBvNTtVmSz32ati09cB7rsXOmsI0n", /*2D*/
			"8In64WEoA44RqyIvsRM7USrULD1YaANxk+4+JW3OhLL0NwGEQytNeS9G+CCb8KVr", /*2E*/
			"fGVlyhFhQxVTSZsWvVSgccNxVHtsnARi6ExnWiErKqtsaaobNRoi0QNGYQr+G76T", /*2F*/
			"C/AoqMUzxzIF7qwBmDkPEXU4hUAIzBSfWK5/YqfxiG77sg3ZMAMGuGC+Rpap1rmo", /*30*/
			"wJ8OMz2oFcowsQw/wzS5MPXF9L/QngYXI3QokvP7Uj+Co83Y1M2zquAfSZLCChsj", /*31*/
			"lahTa1a5KvAV7hzPpyA9E+VwFGqjOUoPa3PVbT1ePlPGaNFJLXod88hozMHTS9ot", /*32*/
			"ZLK8+gZtLReZMDYOM5qib1BwApxrAw0yJRadXrGwUffeemUZOCflPornxqsqZRLC", /*33*/
			"8Skw/ktC4f7GEyiTizK0KUCwGOgPl99A9q9N864qMJKrrpu4dlfgqlQNRGCciMlA", /*34*/
			"smSpON0oBV7496a7cgcoP/YYPKoBwpvlWYH1O4M3MEse2KNehbaGU2/Y+JbUQSsH", /*35*/
			"UTCIWBwyD9GgrL08tSoS9T3Ahe9s8uVwJgEBIQ+Ehd+YUJ6KTJZS+zIVUcGi6XvV", /*36*/
			"JKAi+VBL89SCVBh84Pupa/d6v/4Vfgy5hs9lYDwpBqiS1WuyQ4EVpMr17oX2TfM+", /*37*/
			"0GMlcvRK2SsgOzz3rBH9fLFUERAxxLHAEEHCQjgZhffVY4UQbNqcFeblgIP4s2ZI", /*38*/
			"IKk/XOB8x8U+nwRq91llADmDN+w7h2LqGJ4tBT2TZyE8TcD4jDUEuAxIQR8oIprk", /*39*/
			"lTzBRqN6l942+BKn1jZGCSF+O10+z0XOZOKseAOZgxzIceHcusKJ3gqFqVyTTf8i", /*3A*/
			"g+lww6E6QUPHKHQt4MwGgNMTE/5pID/m49PJkr98tvh+dzUi3Yq+yniBZ4r2UIAE", /*3B*/
			"UGCEpRsC8aHR5D0aoVV9rGAm3hBWIEbr54/3PSUXi6dOW8TUzkKKrIxNz9Iw+BnG", /*3C*/
			"2P/1MENe1NGY5ZkutGjzEsnT3sEHRjOVMWLxesp6x9YyEEOmECenyJwmhWK/QfrF", /*3D*/
			"0fwqs7fC6G9SaCa88WwmM23xYPR359633i5e06mXBq0VdROjuy4X1MxUAP2Ivyp3", /*3E*/
			"w7wsFBFgOVU28ymNqeldB0kNd+jrLousOHlRYnuBDRvOM8s7dqtLTBtYF6wxehML", /*3F*/
			"0mu6lt4s8ZSm5jmx/7ks/GNd6L/cMSdt1kq/SrMs5HFMvmHbcHK/mR62XB2jC7r0", /*40*/
			"tHNMKGH7ZTusr2OHgNiJX7CbxtlngQ96rKphpGC3J73MtgN+TxC/sZIJCHWTGGox", /*41*/
			"WBWxGEKhJt4g3Rau63huD4MhBrdSwD0As8pcnj5TrpcJW+6r1cIvvOM6rPiJkSF8", /*42*/
			"uxn12vB6NEgJ4oikpakgdQ75SDjPMnXKnfH7OvWjroWQ5kX8DeaBiKpbZVNkPWE6", /*43*/
			"bBnnuceYH1ell1X1giVDJVlcQnNbJNeg4Jj/MawrFl+oq1WBeAjWkt/Nopkzaj+1", /*44*/
			"WFpuqKeoblLgxEzkWVz7XwH7eUTSMs1tMcpJInsvonhNHOQQBG9DekqlwkZMqIoL", /*45*/
			"84iBLfDrK+f9ypZyeTl+GGmMQrsZC0V/uhuH2LJzh3sfECmUK4/zqbWNJsvSYmoL", /*46*/
			"aP90Y3q546woijSJdVVxLdWLIGo7Qvka1nrWUkNMhE0KEC7T4jAB+1hf4eCwFnyC", /*47*/
			"/RgNO2W48HgG49UVegt9HQ8ipt+Jmhu2cM7QB5UusqVdTAm4NoHO7x5XIIM+3VaN", /*48*/
			"5jLUQ9KYskBytxKSoVC31v21oME6h51vL4emCWRArOhy+sG5lpZGmiu0JIGjjoST", /*49*/
			"uFjMyI5EweQvIAkmZTXLdvchJgZquJSUYajR4SFqFWAQTKG8Zn7voGlZgh3S1mto", /*4A*/
			"8eNCdEjKvikiKaE642NWcKB5dkwekt08/Nn5D7vncZOYMigpjCgJMvn9jhr/55e8", /*4B*/
			"mMQN79XpwEk1hTtKUCCuIEHZDNc20LGrFOFhnewknRToXVm6bOjxvrn7Xa+gKA2v", /*4C*/
			"xWW/DudxN5qcA6gn0cLUnetkw5MTfhr/zSrbQfxuheuITHqs5LK6PakRxpDgLIy6", /*4D*/
			"caFE/GT0g9WvyqLBMVS/HYPY6BZbd7NXxZHKuH3VYaxmzcoB1grP2tLmDZ3iQVvO", /*4E*/
			"ID0nYoDIT5mbNn/hKiWPGSbv9q7GU3ZwuKPAWZXujPpd8osK8kg4mKBUCGog3I+G", /*4F*/
			"/pw1xMkaU2O20GDNnXUiXLvDyf0la02VDFPV4wQ1mTJKXiiB5cSbs/t2xLwMkxOh", /*50*/
			"oV506WTF/0r34L4tk/JQzRWfMphFmDIQ29n2EW4SQZl0bsCDIZL9rWWJgEQlbZuh", /*51*/
			"8f6V74QQs552+lvN502bcn5zBgKUW0+e8oBqMeKKBWeYm+kwhRuyUdOhdbI4gdKu", /*52*/
			"NysaQMfEI3eT9zqbVVjgVKXlUH5R7KnwQbuiSLalnUIN9F0z/DmAKIQLJ/FfyfOv", /*53*/
			"veUldDKwZk04suwGihkLFcVHzjVMr80tF8qGm/qqoUKkt6WLyhak1QTIz6PG5Uus", /*54*/
			"/W6padhuXpVWIzwNKlZbuNuACchWIl3KOSfq6Te/1tmuLWOhtIll/dN6OVVmqOiu", /*55*/
			"pNnFq1eDE907ia/ivD8lWJVJOrssarnvK+skayqeBasKLp953G3Pf/rnEnJg/5B0", /*56*/
			"snxqP9BTd9Q5FLi0m1i97mXUSB48CH+eA9S0JJus9JlqDBo0txgeyqpIjfqK1CfZ", /*57*/
			"8mB0CeTjxMqh0Yn8Sx+jWfEl4fr4J5B+2BP0VoG2PmSCOsiEX1JkbJKSmUiVoJet", /*58*/
			"PXu50RS2i7kE0Fk5kVllaBlR95KiTh1+0qrMXakHeZgaAwUGTENnA2sMRT3HXU4L", /*59*/
			"gRLA5GEEyZ0OBsG6C0piXVx70fkSOgBACvJW65Ukx70wpK3/xE4WUsYpreuxYBav", /*5A*/
			"0XUJoPMxJs9aYZjFR9lXxLjJB/zJY5OTGIxnSSEucTEr27dMesgzvkjrTCm/5FFT", /*5B*/
			"dzrPAQMpS4iEgAvE6yB8aPpQFHBNg6WdxdKeUt+1BJl22YpUQbS2NtgCIvdlrA/s", /*5C*/
			"Z5YxFcVuY0kKvPLJtiM3fmutznWiMJjcwc9FVSs8wKDs8c44lG4geX3Li0HSSDoy", /*5D*/
			"5xYleZo+lNXhmw8UxdhkXgXapHwcfJAcyd/bJ2mugv9Oc43vN/p4OvVgdL6RBO7m", /*5E*/
			"Yy4zhE4aylilMKUMMJ79OhddDjlhlsHT6qd0kCk5pQsYpCDtOkzXPEtQ7fIDylCL", /*5F*/
			"/vmp7VLQA6CgI7bL+0hAsRnvOk7kLILFlU31xkJTFo2f/x18wm60KqssSN2ujFaO", /*60*/
			"pmaUpXpBr8d9+AbpPGr1FG3IDcLQldwiJMviIvjPMrZ4dikuUGK1H8ZSxltnHDsX", /*61*/
			"hTT1sIRAGkAuTSRGBwDFuWUAVdjnPDVzj9JvU3JMmj2cGj10x11Bvb7WvDqWwerr", /*62*/
			"8yzcStcD0D6VHSkK9iDFFRiQYwdENJVaCNnU5TvGlimGsuYQksFvkYXHZadZjRKU", /*63*/
			"0qAsOkKmMw1xxSP1fO+MdJoWNMqS6VkRodL2U7Rp4YmuZtPsjkNz+DQk66vDvFui", /*64*/
			"a0+BQEOJYUK+UB9xgUpLPA1CuZOvYMjQvAJ0Lrv8tCC16IDYNrsTppmNeLriMbt2", /*65*/
			"2DzlQPpjk2Mv3xEMY3Wx4Zw50hSdwusvmStQRSe5ktIp5TkKd2+1m2aBm7IneRq6", /*66*/
			"IaHAWIYMCaygGQyTDQmmHm+VZNQlq9uXzGx4aTI65mpoPwQIA0O1iRuxu8YseR0X", /*67*/
			"2y9zgsl7TGYoID+raVDHWhS9ZFKUazSC5fElQJ5OAe9cZtedB0UGbw/NH6wGJ1u8", /*68*/
			"+KMYTYQh8ebNJjKGixfhtgPo98an0k2dXnLnC2nLr/sOZ83Nc4bWXXi78HteRDa1", /*69*/
			"GSTl+EjbcDEQNGOB4XrvkEJNMZ14P9tmUtgDG9T82G0CFZDYPyaxpesDgvUaSLME", /*6A*/
			"mDwhR1Ua7NLpHnsB3TuhFOjyMvtapa2vf1h0wWFncInax4QPAEx4LYiyL5h4cOro", /*6B*/
			"VhmfXccAS5mlR6BkoUp3A3K7oLmfyJwaBIGl8nOK+OMM1xBDvmYr5eBdro9eYc4n", /*6C*/
			"zbOz+JGi1MO4QPrdKc/yKrKwaksfqpovU+Be1DqEtS9YaIH6UrhNsfCD5BsHmJmH", /*6D*/
			"VC2tCGYgPsi2MhJ4s7ui3kSIRAsmcdY+Vc1zEgA9p7jzijnDzYmJeoZoQ61fVo4k", /*6E*/
			"63jkwS/rA/uxXsHQF5jXPdfRFc4cRgeMO8q3kZjE55vN1OXpNyStJWx9WwkWmR5m", /*6F*/
			"dL2koYIpsFchoMlW2WXrnDSxCewxjxrCggHXPXJ+3RHcpT8twKcOulSVGV9b+a4y", /*70*/
			"VGZ1fW86bf1TLNPvQSxAf2k99oo9LOHbADWuAytn5JLBvIea5wu/vmKTdhtFyNDg", /*71*/
			"xEdEQlR1QPrK/3jcuW4nOBGhkuNACwPchnm/M7cpc47cgbD+uiDNuE/CBKZA5C9/", /*72*/
			"4VusmFZqZ8TjAH7OBA1s2S1do0QK91qLYGM3/pYX+BqGRum2JygiGeDCKKe6O4PQ", /*73*/
			"MnnkIFa1XSCWuHMESPm8PfTxW+mzqZZTUVyrwT+OoYQoUYmtn68PG++/4WLsanfm", /*74*/
			"fZfQz8Axd1lk1Aczm6nlaOHRdAjKWRrLuSe9g2DmkX8HdTAq8rwEEEbh68Ay+nPE", /*75*/
			"oCw4UvFzLY+iy09IMTbbNoVLU1isfY6r8KzcS/aD3Ml67ASk1YaK4X6uzMsCgEz3", /*76*/
			"OEixhcnns6sMEC6wkWlSHvEuMF92MwO2KIBqCRFyAoKKt98yPLZf20TSaKo2Dsnt", /*77*/
			"VvwE6EyC+NupCQXfzTw+gyfrsggdkxLGOVcOQHblweipuUEObOFGQC9L5DHmRCrV", /*78*/
			"qkE/hqgC93u32QjqFRMcRHsolcJfaVsX53zFHQKnxP+lNQ1cEC4PrsPQISHCyWaE", /*79*/
			"nDNiV3oryKbz0abm24v9w0EbBc92fuOYoJ/TVDJEofwS+4kgPA2C0SsOTRFAeSmm", /*7A*/
			"hJsmW4/oqJEV+Tm4E73lXPmTB1nfutNxXGMvH5p5pWG+/AwO545uy+FIzNwyhVF6", /*7B*/
			"zqzYOMRZYe1k+P9Ss0ctvfOw2OtN/uMAOa5fBUmNSMhQdmU0fsorCMEjT/suJ+8s", /*7C*/
			"UCmqiuy6qQLaQmrPnAjDBLEXrqsFELtWZ983S+u2id+rCymw1+hFxk9Qm1szLf66", /*7D*/
			"MpyswU5yZJgEZU7ul/430PHcGYZXas6wqVHUnvWJoC99iY+6RtinER2dJmi8S9yw", /*7E*/
			"7Ht7KVVis0ehfkUrzSayPQE5lRvNRdq7GsmY2xDF5SR+G/Oeh05e+tl5g3M7LSGi", /*7F*/
			"WU3C2RUP+zoeaTyEWj7jRWX9b7+/z1hZNYHcA1AT+qt+cJX3TTi+Ot+CVyHytJuO", /*80*/
			"R0sA+OjOXXSkzdIKh2a8k3FQJhf+c6DyW0r348PrLVFqQk+6WE8c/UuEDOlUvAIi", /*81*/
			"UVEc7zLkTEgAWVCklNXqGG4dmIjdV4+kCbgWRSoQnkUboogXoMl++Z59U1rCVqO8", /*82*/
			"f30VfBdo/T6jG3waFUVfQnOsaepGiS4WixoxA/A9taDyob/Q8/ZhXOuswdOlPZLt", /*83*/
			"xp3+7N7iUHzXCNpQlgn4F9QDB5SqynvI7Rnsy49JgKvZA/Kf3w6VeJ4DSB+vyLut", /*84*/
			"dbXy5JFkSQhjoqyg0h5tBo/clMO9tWG6UB5/6RJjM+RHyhfSFbt5NPfNlfYT6xKQ", /*85*/
			"a/obRMtDvNxZuelSghDB2YkQLWKHAA1mkjEax1Re4yYiV1Aa7N6VrkI+kM+CUR+2", /*86*/
			"rU4gZFf3A6pDdIYexZWQdnMLbeqZY/btFMwxVN5sO8dAMwp5ZNjMXJfjZM8D414W", /*87*/
			"QsB3xVbyo+W0qSc4DjBBWwStXPyOig/oKZResblmeP/iQWjyQOq8wdqjJbxK/+Rb", /*88*/
			"BEwwnUH7MsmK+JF2SVZ1YmJaXGHoIZ0UKfBP5bIHU2o5AfTTy4e+HKBQGBExg3yG", /*89*/
			"oJCkgk2ScJSDcmwkbkR1bwHxnDr5EUm8vKQihcBcXtGvMEpc2W0ZkbeP54hPDbug", /*8A*/
			"KNGCbeF82skVff17uZbKBybJGUtKpj/WxxALhzcYKNGH4i/uh2vjsbqzSqgNLE9j", /*8B*/
			"YPTL+fOscti9K3ZV3X91FGyzpGDwaqUrjnuBEpyJk9KzYMA45XXS5JewTTzfR03Y", /*8C*/
			"feyiUN09TF37qrDvEKS0RMopvdX/SQY+IIut3eHM2qGBzg31VFm+CUw4YMKvOS0I", /*8D*/
			"lHuip9mmemPsFXSENbm6CSCJ1+AOPYhxu6C1RE3LlUDjHT7ZL4PhxFLSfdLJBx4t", /*8E*/
			"DHLL4hX+j3j3aIrpgIPxolu04Tkq0Y6I07vSr1rrRyGZOVqhx2Yjim7gbqkah9uV", /*8F*/
			"cCQvX8iVsV9W4lzLHUJCsZX1T1k4Yj4tv0I3pB954G0XMp04TFZ/RMhH53bLXx1C", /*90*/
			"TrE2nFTcm1pIvY4C5eVMyE2Qcx1oUE2IXrSUTJIsoqcmlKOAR8qClV7KWNrf+gQu", /*91*/
			"RK6V5ZlU6/ARCw++DHmYr7c4CaXsW1MlSLu9CH9al5HcFI62WUiy1qquurkUMn6C", /*92*/
			"flRXFkCoy/NJzShsW4yILXlsrjkq8tJ/M2wGW7TMQ9EWKpW957ClmB0xKP5eU7HW", /*93*/
			"m0L2WbZCr4JdMdKPdm4sLbHGdU1tCzwd3b4LCiYOvWqagQqZWCX7hsRJyM9Zd52/", /*94*/
			"XKxYM5jAFvncw7fJ5ZAnSdRFpR3vsMrUEug1Jbzz+bJ4pa+yM11TyAGfw55xrGzU", /*95*/
			"Bb+3kGn+pJro8A+Q8FLWlyppLt5E2dHlmzemsy+qOAq2WG0UZR052xJtBoNpcrZ1", /*96*/
			"mWkSW/WE5/eV2qMkjeW39VIPbcY2Epi2oTbniAyxRA12WrT67SUKBueL2PKf6Mo8", /*97*/
			"/1bjg+6zxMvUtz7mnTaDN285w67eDTFqTMROKZ45TYFfhW6SQntBS/JZzI//+xum", /*98*/
			"yU2E5pB9Cg6DZ/tdDeZwyO0tJr1SD+lj2NSFpuENotKzXkIoZqwHw2V9IPYR1LKk", /*99*/
			"BPB0xY2hlaeKx0RiNpogxCrxOifF4dyOcEL3CD5ZhN0TBVMp+3urtkcqVQaShNdr", /*9A*/
			"d0X95k+in+SeXY+W+N83xgwTGsUUfmcTU4j6lp2rfdHeQl0Y4QgGDMyTygSDMvsz", /*9B*/
			"d1BnKRuWoI22ULNgwuJHfibpY9Fh9D/rOv+MWmdI99bmZZ+RYdGHGE/YXPMyMWOB", /*9C*/
			"xPgpb1aImaqAHs7lE1zFjCI4TXIEbzpgbMKBjiv/cHS+/MKbDKwxiCcCcw6X+2Ww", /*9D*/
			"RsBZ4Dd3etKm5tybDLHlJ3/HNl1Wgl9zDQtcVE9Ka46EpaH49azSCzNK+9qZpLfk", /*9E*/
			"oqNnTmXCEyAYPJT2kIp3OokhKKD6o/4D0x1eZFChCBiK1KkFUPGd7SUqVrr3ZVqg", /*9F*/
			"abl09ZCLxxawdYOxVESlqk1kgyJDay+zrd06D8sG5XLtwDD8rM8lP1IbZHddIk9Z", /*A0*/
			"PKfgNI4zij0pVanntZEp+z6BqsB+T7yT2NINy8Vzcvzcceiz/qrYb5aNK+K3T4gV", /*A1*/
			"C6MfuzZPzeuI+pzH+xJl2oqLL5w8sQ0pgRHpcQ3el0PooA4PxirHkiWzIfuTPtVr", /*A2*/
			"RPwn6mnipwEUzuInbZruuezITfSwOeWcSCCRo2YLdpPHIhRl3qRAiUO8ajfiJZNH", /*A3*/
			"rXDoPq/ReLm0v8K8OSAQGqe1L/hn5Cb+GBU3iueqonoMA/4OmEnAplXPuTFaFlvO", /*A4*/
			"83te+g6neeAycMwB+BUSWS5qQTFSSOvGtPc9MVBVAXlxkcZweui+bTCYrY87Iqmz", /*A5*/
			"CizzayTFZR6IrBJxiHFvdDqATB4N7mg3KkfZF5JaKBPaL7nFPmXTUBYbHBIlgdDT", /*A6*/
			"4MbXp7hYZrImWKf2uLq6mC8eV3xb3FwMFTx84jV8daAjU5+Y7KVMWoExNdEXCQQS", /*A7*/
			"ziAPHV0/PCj6gG6n9/7vNXtgCrZvwoTCq9RRMZdBXPSk5oHG30kE30ApvkQxkk6f", /*A8*/
			"CWvFSIO6fqO7oVAhr6WkGwJehqHs4jiFT7XmlPDFcOzMHC8+Di9TKGckUoyrgaRq", /*A9*/
			"7HT0ZMzLa8uOyZmiLclqsBhA79HXqgNn4r9DecDJdPZ7mtZiufma8zFG+tmIPFYs", /*AA*/
			"ic9q6wPvn5Hj2wmG2iJ+bJ6t37jm4qEfzmznhDXMlasV/rPCqNoEHZqInx96AEMd", /*AB*/
			"dBcIiVeOgecOnmWvKURwwG2W2bnsynOjk/ROYPtbsQnr2DCNHG1BN8GwW8kMnOcU", /*AC*/
			"CwIqFF9HT1Kar0suulFkHG6vGkqZnVObE9GXL+RfJ/TDCedhfcAnxLIV+S8i38wk", /*AD*/
			"uGkWjX903hRDLdaFG1MOTo3hohvXp+J4vz9DsYlrlB14k/hz7YfjAMYyqIOCKplS", /*AE*/
			"nQAhy/tp28vqaYJO7P3GDj/R/omqCNl58D4QoVPlSHVEyV4RigTku9HfvX+/Dsbh", /*AF*/
			"t58h7KYqUiTa3d6l6FLJePCdkpfv+SIogrZYgh2dDKeTHjRtZKw3iw5evUv/NHzW", /*B0*/
			"WkK8+iLNcOdcB8Ws0zScfiNmx3uMp/8K8gMshVGY7U7+JgTmbuwDck7cIlC4zxdl", /*B1*/
			"oZf5UJb4gRWe0wf0eemc2z9/NDkOgNQKLznRn41bw8BDrmdhSolN58GayuP04mLU", /*B2*/
			"h8dBW1xU6gWQxgRhxS3NzstjyWx0wDdzsbg28YGL8P2QKcdZIYZ6fy7tsGwbEdkK", /*B3*/
			"ZwOSKgN45W9kIYNc5a7vSVfvWjaXeRWze8YDyRdJ+Jjze6Yq2nXPdaTYg562is3S", /*B4*/
			"oDXjxLWeuWWAUiUnknVqSMDqho9VO3pP8NuGJsyJ3wMRo+pOFpNWqQnbW+8HkxTT", /*B5*/
			"QXBSKbVhMmVBkniayp5Vmnpn6AkO/zFZ7FgU9P061fYe+GvpQkS5An1cqR9GG0pc", /*B6*/
			"VmoJ87M9+ragQfFpg2cerYhRXu2pjXCB7R2oN9Mp7uLAN5us8HTs1zdgx6NvWngb", /*B7*/
			"M+0CgmE9Fa7dRl4YFRHp6LxyQzY0utkFPeHbm2LhLmfLYUpiqF4v/DVQkuOLGUPY", /*B8*/
			"ggUT/MH36LxytMXFva5xpZwekYMFxwh/WNmpleoWvPRyLOrdaz3uqYP2TZl0G2Ub", /*B9*/
			"HUbqFtrZpxOxMtxO/ATPf/iChRM5fGBmkaBzrW5UMYj99dFJ88Sk2Ns4FnDVyXVJ", /*BA*/
			"j8cOH6SmCr/zJgJJT/Yr3Dg10PKqE6PNY1pZYETauAS6Y7ebchs/onJy93Nqc5P3", /*BB*/
			"Om7fv640ozuy5aH26jcgIeBBvyAZPEsAanBiqFsbodir+BVzAuV5ZT3hVG27nB88", /*BC*/
			"16KivQLeJL7e6SfgSKZpTcWQvXNTKrz+2xSkVQ6sx0LcnmUht2JZ1iMN+3MuG4NT", /*BD*/
			"lA4IlD2M5j3ch+BoyAueHVyzcELxHq3n5WcPPjfPFJoA5rfmq5jvAKpOIxgfPzWC", /*BE*/
			"HPUsk55UNGAH1LQ+buPiPD8esyLKgLqf+JOtjKyuLhTynJDlplw9XbUltoK0cCwb", /*BF*/
			"OBlF/nciIbza9D467zcW4tV6zlXRcJzD+DofSXjcPlzfFn2Em1xAGnUYKW4Fa0QP", /*C0*/
			"3OG4WMeR12nKsLgSRajYSbwdBl6w93QkbT9Jsf+XnKdyl0p2e+e5b/7YtkxhkxUx", /*C1*/
			"J16k1XeNf/ZyZdJYfyijF4Ag5ZXlkicYkDo3LAW3FX0zB5Frl/JsUwTVyzq0g+Tw", /*C2*/
			"7ieO0FHjHp9QHCUBAY8Vdvpn3i/SnybBXj9vbqIwGWxxsJajqqAqo9mLkxaUWXpJ", /*C3*/
			"Zxd9jvAWmVYlx2XxRvAgq9oy3sOvHEy930wnyhz2Gc9590YEyb3rTQ6bQgcuUgcN", /*C4*/
			"H5LzcvuV8rDugK2fp9+SxqaNI+ApFgFpLO4KkSKvRgC9nUrJlLved6gDiu4QEUBL", /*C5*/
			"i5/WCMovbf/PWipx8SpZl6YMuwPxClSdEI8ycuUzWdWn97Avyhhy7K1SMVjOThWN", /*C6*/
			"+o6xu6O272xdu0itNwR65LRz64zTlZasBmmwzrZoVE4fWfFDewBXOlEmHNIO8e0W", /*C7*/
			"ayrprkyhEjBuezkWCi3Ox7Y23WxnHc6yRT6MP2j5Y/VDlHPuQyMRPHtFMQw4Nv3S", /*C8*/
			"6Imc5+3XghCXxQfYiMDRV6/ycsfM6Vye2bgb9srRCtOknJOmJNBAHgTxjTXr24VJ", /*C9*/
			"5MWbwD+/bmHtZ7NQ2sdJqbnNea/4nigMaWQ5XzowI1y4nWpVf3IWM/zDKTKU7My3", /*CA*/
			"rl5GTcC4tLmYd6pUbYNE7Y6R9Q9f3PemI06O9sDRDXCsImJRoPryS0KGNAJg4io8", /*CB*/
			"KlN5i4fOfop9SWsRNjhhI8cKwQSYXmb+21AdYEm55kWSrvGHSJ4qW3kg/6RL+X9E", /*CC*/
			"YIBYHxvtHOPnG9VwzhgK5vPhogEeisBifOfqh9azjgAkQhNU8qcBHaDzVm7newB+", /*CD*/
			"S4wdnimVGeOXzTthUOb70W3ABqg2Pzwdxqn02cN48kkZC+pfjlGsDtpuoMmQozd9", /*CE*/
			"PoEn2D4qj+N+VHXSCUzNxda+Kj8x6PAcXmzP7Efoz549JXTJhIHmYPhm3Wxjt2rt", /*CF*/
			"g+xNlU7cybZ7Ex481IZ6M/LUT/3sXBphCkmfJcbgq0/5/hB3EZNDJ5G1Kd7/ZAr4", /*D0*/
			"NoGDR7SVucrJmGnWIO66+AcrTTaRhn8DEPhJSYL+EfdE6bRzJ04bDSleT8X+JLr6", /*D1*/
			"zo+WdCkLjDD65/1kK+hcMAecx3r6V7nLsV98nWmxi5NZ2LRS1l/8TVSzmh3Cf23H", /*D2*/
			"ubK23iqdPyHmp4uA3hpQCh0SWK35qfHhBwPT4+lM2uEuwlG97vCPT/ACycB5Xa9e", /*D3*/
			"/Sn6hpRr2GYbiMZXw/pZrKY2bqOVVcVr7ANRMW3wKxb3Jh9eqvZsEiojypzEgPCd", /*D4*/
			"/kKNPMrboiYpa3T7QMudoLe56bMGdwDLN3jIJ4UkzqueYUvX9T1egjCMtZyd/Nfy", /*D5*/
			"4xsUIbpGpVyYqjTXa3GIRaVXTa+a5u5KNEd/2MvYHuC2TxpgnaU6JPXg57tNx6w+", /*D6*/
			"En7TGH3MPRIDnoRZm73XqZfIE6YDokSJUsQu7nEiYRcZIl8+IxzBz/D9+LDhfaTw", /*D7*/
			"rChY6DsmhE3Pk3MP82hXxHAK+PfAxEwN/mVphN2AeAHMjea7LZ+8wzY1qkfixzbC", /*D8*/
			"w1Tat7jktI4JlDjRpicU+rnwGYurSUR0lNsLLSvj+2v1T2zgtj4xkY1XeOhPpnAB", /*D9*/
			"9ZQ2fsEFnheLL+eprp1ZMfMnIDWxBIcCmvBuW5f0ttrnj268CnM+XtFjR0FHNnVB", /*DA*/
			"irk0BC1Put2s6trzIsW6D15xMbvUrhXTJVSe73g9H1zrN/aLLVTduBzOleDuoqhf", /*DB*/
			"Gt9+ETucUYdGbjBLkmnE99Y/6W7VWcjS+0QotF2XQ35OOYME/+XstpgzHumVJdYC", /*DC*/
			"vMfUHxESaDJ9GtjCokSVCUnP7qwSuym0yF/RqgwIFvnA4HMEGb4U6BD0pl9spkWC", /*DD*/
			"Bh0YMx31KHz9JfLWrL/hwspYsD83d3RttA+6rKQgMScvP7yQIeJA5zCIdWUH9ffr", /*DE*/
			"sR4VXGa71KlXG39XMuqhxnrI0buRACrxpQYNbSbr7VIGg9MieUWCZuH/gawDgCCe", /*DF*/
			"7hTMj/9tRYg4Y9H5E2jct0DNGWebd0x3zrZ8VmatHkJhc7FPb+YE72fuuJX5B4xw", /*E0*/
			"CmYVUgZsUf1tQnwPXvyXvInjXLXa4nI9/YlyS5KXu0il/XhlR8PbAVZtFCVJtGRU", /*E1*/
			"NkBz/ihftBI8TEsXLYWrK/oq0aDw9dPwsY/4wI/OCT4nAmnPBeQruqybXq2tzQAx", /*E2*/
			"8/DLFizbxUODFq8Ae/dYQUpLrtCISI8EXx41FfiYrTYCeh+vnDDWx8T2tEVNUvxO", /*E3*/
			"w9IpqeBgTyV/2HHCtOaqrqoCojpxDNzQfgIDuNoEUNch9L3ykP4vgnK+mZWd/5iZ", /*E4*/
			"GECqPT1v8vQW688zR8yjY90LrmDrCWJZ52PY8Pm3vENYf3QfldAFZUctEHof1zSa", /*E5*/
			"eLqaVApmBmqQsVMTCPE+Jg17pUsjNWcJwIzOkwISTh1EfHAq2DxKTjQpNwDuJ5Bp", /*E6*/
			"2ttw5vyDDQ7eed83HdcEnwdmRDVQ00OUV4SIzn24HxAYfAUkkvd7YsvMP5X9AEDc", /*E7*/
			"peD/ydYBaf/vbd8F363aLN6769N29NqYssVCzjVx05JekFHLcn0YRUJeRKwowzSb", /*E8*/
			"MqVxGCJPO1nZB0o+x+5YW6Y7WVGbV6MU+zaGuWodfC1xQS1YxvNRm5O/+8c+4itI", /*E9*/
			"MskPPh2qiM9kApuYN/AYIZyercSDrH2zqiuqR8yU3qzNg81rwjmIEdvJkzE6hLUy", /*EA*/
			"Jkyc9euUNifCSA3l3m7oQ9s8ZO2TcPlnzq+H1wFSr6T6VaxDMp/oiC9el4TKTMpI", /*EB*/
			"NJbZkDc98geePpcJDBdSUx8YCNLv6/dPuH0XjtR+CV3V7SVC1TMyyM12DDYamLK9", /*EC*/
			"rWMfwjW3e79xyZobuOwq1CudYVaVIIRBN7dKso0I+1bN107MF5BF7/PUAgF1Tolo", /*ED*/
			"TIZGFuEJMOyzlokhvm63q4Q2Q4vkZkOkWyCPX5vNUAJqaoZ3IQL0N9+ZvUa0jtwh", /*EE*/
			"2xMDpS0Vpg6iKn97vfwYS5FA06QuvSClQTQyNgjCk5Ifi/xEP6ov94d6hwwiqeca", /*EF*/
			"h76oi+zn8qL2LcTqOE94md6lVB/79RjQ6qiJjxKK244lCl8AAf306YSjQ/SQUIb2", /*F0*/
			"yLZEjgLxjWROpY5YEq6aI7JX7IoJRTLHDrbU+DiZtjmZf8wYoc2M6a2ZyCWcNGRl", /*F1*/
			"14yuEBkHuu7rjHFE5NQTrsBuESfqzDhaA5VEuVD5JmYtKtkIDL4gTdudhJrp1pnA", /*F2*/
			"6x0rVwnxbbknNlDvHvpOza5mhxMrw5Igyxz8+gmEc1/Q2xKHn+/OSHnG+Gp26CBK", /*F3*/
			"MWVHd4YN+hP/iiH6EQNW9hnOgCf1nYLd9eSZPd8TKqjIRHh7ic9wVcrMbM12uuC0", /*F4*/
			"iMptWb1YY17EHcGXSaiAxJMnFsSh/b1EtoGQNSBZV+qxqELevU1Az/znX/cGKrdt", /*F5*/
			"aY9rB1ZB/i8b24U5Z70IvnckcTgjsfsuI8LLhsrG2sMhGVzGHRTwAAgWuBZoVUtp", /*F6*/
			"PIvyVyTvDFmzKOwVYcW2CbCmufAeaDg9buhp8ofmbl2W9NcE6R/gNudWelVZTDh6", /*F7*/
			"9KeHIw/sBhxNld8dWEHEoUfM4dQlFPphvZgRgKecfLq4RoD0iXOcMFY6H1jzEcVs", /*F8*/
			"VshA1JaS9FA1uBlz8tosl4u3piqGXTUtOjSz2q/FG68UD67oEhxCS2DGGn+VLVk3", /*F9*/
			"0w9eJqZcpKB1y+h4as8jRrdXXlZMhlXQnr8DWNyHFJsky1UbVl+42X/mZEoB4TlB", /*FA*/
			"w0B3Y7qqNCbK/uvfOg9J60Or8N0vk2i5sun9/EAfSEolp5wq99MmTpo4o4VMD7lZ", /*FB*/
			"H/DE4bOdhS1JBfklaMuN97w8hHlvnWNeZOWwMtpmOAYjafQycH3BHRymJfjYn89k", /*FC*/
			"SIbcArp93tJ4PEL+TguGcO47gDxBRggT1ww31xvl75eL9HY3xlByZ40Vardou9G7", /*FD*/
			"O5uhd2nhnjoOErIRHr4p8rEdw0Pk5rS1vv2VZZx/0D00fFRnq41JtBRjFrnR9EOr", /*FE*/
			"M+Ld4p0X/Ai6FMaAdoKhvJxFrw0oKVIspycF7MQrUhvN2yO/JVwShPuDPxFE0FB4", /*FF*/
			"vcjoQTuOlh1n1+57x7PwUgqGn0c9Ixa5Vr7kINo/685rjPJaonSP60lOnrWkP8G/"  /*ZZ*/
		);

		static final List<List<Byte>> MIX = createTable(Byte.class,
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
	}
	
	//=======================================================================
	// HASH STATE
	//=======================================================================

	private long[] hash;
	private byte rnd;
	private boolean finished;

	//=======================================================================
	// PUBLIC METHODS
	//=======================================================================

	public MHash384() {
		reset(); /*initialize*/
	}
	
	public void update(final byte value) {
		if(finished) {
			throw new IllegalStateException("Hash computation finished. Must call reset() first!");
		}
		final List<Long> xor = Tables.XOR.get(value & 0xFF);
		final List<Byte> mix = Tables.MIX.get(rnd++ & 0xFF);
		final List<Long> add = Tables.ADD.get(value & 0xFF);
		iterate(hash, xor, mix, add);
	}
	
	public void update(final byte[] data) {
		if(data == null) {
			throw new NullPointerException("Input array must not be null!");
		}
		for(final byte value : data) {
			update(value);
		}
	}
	
	public void update(final byte[] data, final int len) {
		if(data == null) {
			throw new NullPointerException("Input array must not be null!");
		}
		if(len < 0) {
			throw new IllegalArgumentException("Length must not be negative!");
		}
		for(int i = 0; i < len; ++i) {
			update(data[i]);
		}
	}
	
	public void update(final byte[] data, final int offset, final int len) {
		if(data == null) {
			throw new NullPointerException("Input array must not be null!");
		}
		if((offset < 0) || (len < 0)) {
			throw new IllegalArgumentException("Offset or length must not be negative!");
		}
		final int limit = Math.addExact(offset, len);
		for(int i = offset; i < limit; ++i) {
			update(data[i]);
		}
	}

	public void update(final ByteBuffer buffer) {
		if(buffer == null) {
			throw new NullPointerException("Input buffer must not be null!");
		}
		if(buffer.hasRemaining()) {
			final byte[] temp = new byte[Math.min(buffer.remaining(), Short.MAX_VALUE)];
			while(buffer.hasRemaining()) {
				final int length = Math.min(buffer.remaining(), temp.length);
				buffer.get(temp, 0, length);
				update(temp, length);
			}
		}
	}
	
	public void update(final InputStream stream) throws IOException {
		if(stream == null) {
			throw new NullPointerException("Input stream array must not be null!");
		}
		int length;
		final byte[] temp = new byte[Math.max(Math.min(stream.available(), Short.MAX_VALUE), 2048)];
		do {
			length = stream.read(temp);
			if(length > 0) {
				update(temp, length);
			}
		} while(length >= 0);
	}
	
	
	public void update(final String str, final Charset cs) {
		if(str == null) {
			throw new NullPointerException("Input string must not be null!");
		}
		if(cs == null) {
			throw new NullPointerException("Character set must not be null!");
		}
		update(str.getBytes(cs));
	}
	
	public void update(final String str) {
		if(str == null) {
			throw new NullPointerException("Input string must not be null!");
		}
		update(str.getBytes(StandardCharsets.UTF_8));
	}
	
	public byte[] getDigest() {
		if(finished) {
			throw new IllegalStateException("Hash computation finished. Must call reset() first!");
		}
		final byte[] digest = new byte[MHASH384_SIZE];
		int output_value = 256;
		for(int i = 0; i < MHASH384_SIZE; ++i) {
			final List<Long> xor = Tables.XOR.get(output_value);
			final List<Byte> mix = Tables.MIX.get(rnd++ & 0xFF);
			final List<Long> add = Tables.ADD.get(output_value);
			iterate(hash, xor, mix, add);
			output_value = (digest[i] = getByte(hash, Tables.FIN.get(i))) & 0xFF;
		}
		return digest;
	}
	
	public void reset() {
		hash = Tables.INI.stream().mapToLong(Long::longValue).toArray();
		rnd = 0;
		finished = false;
	}

	//-----------------------------------------------------------------------
	// Convenience functions
	//-----------------------------------------------------------------------
	
	public static byte[] compute(final byte[] data) {
		final MHash384 hash = new MHash384();
		hash.update(data);
		return hash.getDigest();
	}
	
	public static byte[] compute(final byte[] data, final int len) {
		final MHash384 hash = new MHash384();
		hash.update(data,len);
		return hash.getDigest();
	}
	
	public static byte[] compute(final byte[] data, final int offset, final int len) {
		final MHash384 hash = new MHash384();
		hash.update(data, offset, len);
		return hash.getDigest();
	}
	
	public static byte[] compute(final ByteBuffer buffer) {
		final MHash384 hash = new MHash384();
		hash.update(buffer);
		return hash.getDigest();
	}
	
	public static byte[] compute(final String str) {
		final MHash384 hash = new MHash384();
		hash.update(str);
		return hash.getDigest();
	}

	public static byte[] compute(final String str, final Charset cs) {
		final MHash384 hash = new MHash384();
		hash.update(str, cs);
		return hash.getDigest();
	}
	
	//=======================================================================
	// INTERNAL METHODS
	//=======================================================================
	
	private final long[] temp = new long [MHASH384_WORDS];

	private void iterate(final long[] hash, final List<Long> xor, final List<Byte> mix, final List<Long> add) {
		for(int i = 0; i < MHASH384_WORDS; ++i) {
			temp[i] = mix128to64(hash[i] + add.get(i), hash[mix.get(i) & 0xFF]) ^ xor.get(i);
		}
		System.arraycopy(temp, 0, hash, 0, MHASH384_WORDS);
	}

	private static byte getByte(long[] hash, final byte index) {
		final int pos = (index & 0xFF);
		return (byte) ((hash[pos / 8] >> ((pos % 8) * 8)) & 0xFF);
	}

	//-----------------------------------------------------------------------
	// Utility functions
	//-----------------------------------------------------------------------

	static final long KMUL = 0x9DDFEA08EB382D69L;
	
	private static long mix128to64(final long u, long v) {
		v = (v ^ u) * KMUL;
		v ^= (v >>> 47);
		v = (v ^ u) * KMUL;
		v ^= (v >>> 47);
		return (v * KMUL);
	}
	
	//-----------------------------------------------------------------------
	// Table initialization
	//-----------------------------------------------------------------------

	private static <T> List<List<T>> createTable(final Class<T> clazz, final String... data) {
		final List<List<T>> table = new ArrayList<List<T>>(data.length);
		for(final String line : data) {
			table.add(createRow(clazz, line));
		}
		return Collections.unmodifiableList(table);
	}
	
	@SuppressWarnings("unchecked")
	private static <T> List<T> createRow(final Class<T> clazz, final String line) {
		final ByteBuffer buffer = ByteBuffer.wrap(Base64.getDecoder().decode(line)).order(ByteOrder.LITTLE_ENDIAN);
		switch(clazz.getCanonicalName()) {
		case "java.lang.Byte":
			return (List<T>) buildRow(Byte.class, buffer, x->x.get());
		case "java.lang.Long":
			return (List<T>) buildRow(Long.class, buffer.asLongBuffer(), x->x.get());
		default:
			throw new UnsupportedOperationException("Type not supported!");
		}
	}

	private static <T,R extends Buffer> List<T> buildRow(final Class<T> clazz, final R buffer, final Function<R,T> getter) {
		final List<T> row = new ArrayList<T>(buffer.limit());
		while(buffer.hasRemaining()) {
			row.add(getter.apply(buffer));
		}
		return Collections.unmodifiableList(row);
	}
}

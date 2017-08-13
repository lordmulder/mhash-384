/* ---------------------------------------------------------------------------------------------- */
/* MHash-384 for Java 1.7+                                                                        */
/* Copyright(c) 2016-2017 LoRd_MuldeR <mulder2@gmx.de>                                            */
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

package com.muldersoft.mhash384.utils;
import java.util.Arrays;
import java.util.Collections;
import java.util.List;

public class JavaBytes {
	
	//=========================================================================
	// INPUT DATA
	//=========================================================================
	
	private static final List<String> INPUT = Collections.unmodifiableList(Arrays.asList(
			"01DCDF00414B3037B1B3AF661B8E96F8944D2873DB39312173DA9A36662AE7551F4F318C4ECB56B1F09743D99C2AA5BC", /*00*/
			"A81FBBC6CBBFC95439DE43648959EDDB1A641A0BDA01822FB52E6072669326582C5B1731AC802084C2EF10671FC79DD4", /*01*/
			"CF2A8D8E088100468B7E9B2089E268F6930461652C5D2DECF096E42223BFC8B98DD338964CFE0970269C342F7CEC60BD", /*02*/
			"B970A920D2ED0552010F894A254DA7E18D5F205F9D1A40D850C33DCCC3FD5F58B49F31BDE7D82C577CDE04F62A959033", /*03*/
			"49FAB2822B9C84AC34B8648CD68CBEF1E5121147BB4126DEC0B31F54B2FFE00F2F193DA38E8CC632058C984B429B8AFC", /*04*/
			"E07F9DA44D8C9942BE2AF9B39ACA65F25D3D8FB1466DC295C0051E3EC3F962C7F8FDC3CCD4CE2BB69024C0EFC6199937", /*05*/
			"A3111377EF01F5EF31F59B366C02A3FF61B82949760D16DCF6B958AF92BD3BDF7297AAEFEC69C0B2FE8A50AD9E8684CD", /*06*/
			"3D3DD0C829EA9AA03E77597EEC96C7A38FD038231E7F168464617B131FB7EDE085C99CE4C5405874A58735D41F19C5E3", /*07*/
			"8028628ACAF91B9A194A640538C9706404A15E018A4F1680F4BE1B04C2360955DFB24D4CEF581A203C59D0A0FD15879D", /*08*/
			"88F8E2ECE78AF1BCB46B6E22297364C29339F17F926B99F331293489B3B8F07C3909CE1649C9BCF10C46103BFA31BCE9", /*09*/
			"4D1A0B8CC7EFE2A9AFD7878CAD55E871C89CFBC858CF4153C4739486C74F75D60BF7192C130AC9F20084F2BC5E81BD9A", /*0A*/
			"4AFBE975014FCACF41DEAF8CFACC41E43C1EC23B53ED16E978B06EB30F1C3248DD11165D04285C726546D9B5609491E4", /*0B*/
			"FFD1E2E04DC8D26007B186948A74ECCEB5120E45121AC531BD0BC86330810C85DE93AFDDDB48773078DEB0DE6EB99196", /*0C*/
			"79BAA2AC3FDEBA55B1B7E3E1C92A567EA53F69AB4A5B0794F0DA7499954F6DDD58117C89E1132248D95DF5F794C51A6E", /*0D*/
			"89BC8A0C65091C33B04EEAEE063135C2F53267E04AB6E689B4C579B7207E8BF23FD31E51343CD2DF119E523E2F8197FE", /*0E*/
			"2E10CB1C6060F32DBC92E732A94E6B63F3220D831FD04267502C5F7414BCE87F89E0651DE91D2457759E56B04482D915", /*0F*/
			"43AEE32C6A84E8030C5007202C0BD7E3B4F464474205D32A7D17FC95DE386C06E8DFBF64567AA54555BD889D5853046F", /*10*/
			"687ABE14EAB8DA27397B3AB50D72C344505EAA6D1FDE618D61BE79865DA13F6917BBAB29B5E90E2D010C921972FA8B2F", /*11*/
			"8B7223A4F56FF453291B7B5CB98B6FE1FD421625786FBF7DE33D1020D0E8CDC8CA530C708B739E87878AF1F304B8A12F", /*12*/
			"7ECE1F24E56DD7112E0869241B2FA6DF84B834DCC459B2FDE022EDA6319E7D3B59839D8CA03C9928644790F491BBC774", /*13*/
			"CBDC6F49E6B0DD0D44BA2F8D0034673286BCC821586AE61CC7B7491285CEE55BED3912FFD97F3851F4AF0186BEBEBCBF", /*14*/
			"CA8A48E54ECCE516BEDF1864B7F8F506D56A1F60A4B36AA449B25AB5FE0DD9D921377DBB5E49FCE1708F64F5D59D99E7", /*15*/
			"E9F873A569AFE02CDA66BC05CA9973908C88174756D35385EAAAF16CF4FDA730B39F7A55653A3512F10AB727BC23E852", /*16*/
			"93E96FF8C5BBE2AFA47785420253E97C704B25D1F77D074CC0B2093F1470559C8D5BFDD4E364AACF550518673F5B1BF7", /*17*/
			"8DAC832E5BE81ACB588BFB202B7583D8A34D8A70DFD1D7E4D03B0627B687033EE3D0BE7EDBDC75CF818EE8C5B09F8BEA", /*18*/
			"8E3B02E1489D7D315336752B64E3B532E4D4CC795C580A656DAB08F313ED767E8E567E88FDBA36BF259490F1D8E933D0", /*19*/
			"EAB437D0D62CAA62C090AD28B982B003E255D768D25704ECA048511AF6256A43E79F078F4D498B5FC41735FB75B357FE", /*1A*/
			"70DA9FC3504FF29DB9AB1F388673FF2536922F4CD17357BAF09C3AB292E7E04A90CE0BC3D9BA13EC647C4CA63C918DE3", /*1B*/
			"D834A38DD1ECD688A97A0020DE46AB6A9CDEC7F6E62EA71F288A5A6FD74DC47ED44A2E2765CE50F1BB9B50B5DB9E4F3C", /*1C*/
			"C66DA90E41DDF2E65A3FE86F160C5C096F6AF0405108CCBEF938382EB627FC7F163DD634617F006C5184B1FEDC908497", /*1D*/
			"C95719ED07FCB21C5112DF043F6EE7EB50F9FD60012334CE589FA85104D96579B7129E44D71905A73314766E0733528D", /*1E*/
			"DC6C8014C5457CB8D635FDCD286A69B6D66F232CE27D01BF56AF4AC0F682EC0F57DF1D55B64328F5651ED4C52A87CACA", /*1F*/
			"26D9D1CC79EEC50269BF340A34B1EBFEFAA5AAAC8E397174D0A8F9BD426BCF6F5B131F464D6D2452122DD15660D0D6DA", /*20*/
			"6E389AEC5F51A22F7EF68F1C54C127FB986D4D46E0485C30F0A47B39E7CF8A311D398DFDB7F2A78F2FC651D1FBB10D2E", /*21*/
			"A44E4E8D1B49DCB007A4822049C2F343C40AC04A8D6505BAD9B91D3F0729B16CAAF39951B50F9015966EF5D3AD3F9076", /*22*/
			"EA78CBAC0EB6D009A0FEA6725A23DEABCE729C7444CB94D940A994626627AA0D37F738CDE3D018D54C29491C01CDB3C5", /*23*/
			"7C9792AEA745C87AD1FF5620C5BD8FD49ECA84E3004B56B95AFD3923C228B1D6E5DBF79EB3FD283B441712E354084B9F", /*24*/
			"E35D288BD8E249BC91776C1453A366E5F5D1E1684E95EFEC9108E117D7DDF60681B30F9DA2CE7C8C6497DBD786818C0D", /*25*/
			"C2F891FF45044BE075A1A76D2B87E2EB85CE65798AF3C2BF213F532B4EFD09DC0DAA1DF5A53A6C883028606A50D826B2", /*26*/
			"609A62616379F33AA339A3BC53E4516DD7AD92616A5ADBECD043726D86E924AA8555B564F4C2986556AA12AB31C1D6B0", /*27*/
			"CED9ED85C1C17BFFEB522325ACBAFFC204D3D8F4B2D15394D271504C04756EEA2DDBA4A91AF827F11F67D5C28F8002E4", /*28*/
			"8B9C08AD432DC08F5A7543E29796BBC734A6DB5B3C1967DE016E3BC2A2804EE45B9BCACCE5172F757549598B80ADBDBA", /*29*/
			"6F3FB117C5CDD15516C3B0A59CD6EEC5D9A1A411DE538769938C54979F4AC37C3737BCC1D55284DB6CAD9F8AF9156BB7", /*2A*/
			"EBBF284F9C75EBDFB383EBB406753DE8AA86127AEE7C403C10BFDD10523DE027138BF6C4EB4A8A13B1EFF67DDB78B067", /*2B*/
			"F6D1138D7AA3DA5EBAA8098D8FB66371DAE76D1B8B6CAAF2400F1034368D1EDC7C937F5172E8D2777D05BBF83CADE6EF", /*2C*/
			"0E9C2EA6CF34B0819036B30D58F60BA0DB3A2C5848F08BCAC87AD1B94250D5647C892E09EEF9616626DB85CF571085F3", /*2D*/
			"251EE3F58718C12AF9438D81178A2AE4F0929A889039A8A8F06B65225EBDCCFD2E4D14EDF7BF73C6A9369895BC1DFACF", /*2E*/
			"CAE302B41D6979CBBBFA5A58B51EE6235113B99DC81AB52F6093795BEC17A0568F71FB4D2E5E355E762F92EDBA34A2F2", /*2F*/
			"D130015265A4D9FF09BEA253D71F26C881B6EAEDC46521E6FAE268165682B8A9A89C3EC4774AB6230D2E45E055219DB2", /*30*/
			"2B560284C3A692AB37008AD0B379A7B8AF11CD2C30F90BFC7FE87A250F2762EDC3FBD711647242C374ED8264F6B322BD", /*31*/
			"28195CC8A7AD394353CBE808464C4FC4D58E3D7A765F8726D83052F60185AA4FEFCB0D85223BB4E75A31305E787FAC28", /*32*/
			"725D0EE230F195439091D2C6BDDF34E0E3BE49C6C275460161BE300BA4AD566B02D79D7551FA7CC16543910F5F1CDA58", /*33*/
			"4099ADC44481B43FFE1361922FD9EB81A989C09E441FCEAC449B3A13D3CB901945A9BE396F201134DC1AD05A046633FE", /*34*/
			"1A563A6D522F3E69BE589E079F475A9E75A2A9638E4C0038DA3B6202577A0362211D3F1E0D727AF65E1FFC529AD99233", /*35*/
			"47B61E86C6D6D01B437D6F83ADADC318D5A361028DED738CA00D4C630425164B1A69AFA5AF4C9DD2F99E1C67F951B582", /*36*/
			"A66A7740B6BDEA79FEF7FF1496AF80A305AFD43EEACD898CB00C78ED31AD71340ED31A1AD784667374B96844161499BE", /*37*/
			"46FA8D6CCBF6D12E31C2FC147F303956707F4401DE5F067F3AE5FEC7E33594E928E39F8A63531714B7B329EA1E9FCAB2", /*38*/
			"EFD8F755825C78041F5A93870BD30CD1EFBF894671FF871628ED617FF22BDA58411289CCAE5CB62E95DD42F41801F2F9", /*39*/
			"A8525B8645FC59E175E62DC00A5F7F0C09C56785210416AC50EF76E9B30D76262B3B2CDC19F5D665A41297CD11D8F4FF", /*3A*/
			"EAC99A649EEE5039A593C92F143C0065B314735203071206EA2761A0C764A4EC02AA7FD46CAC25B3C68CC182A96D03BF", /*3B*/
			"B2873F024EC83CA897470AB8FD8853EB18FE15C159B305BDB0AB08F687EAEAFD510A3FDE73602E4303E1B84DCCF0FCF0", /*3C*/
			"D85BBBDC8033C0D89223D9C39CA9F34F7D3BCB6D5B63C3FD1C30F974DA0C0FB58B24BC9EBEFB5143C58954925B7B84FC", /*3D*/
			"6ABD7C2E0844D7A7CCF2EA456CDF530DE8938CF52B3921B8BA023CA2F281657CEC635DA675D1EDAEB4AA52F22EE1BE6C", /*3E*/
			"981C3AC677CB59046A92B54C84877B49745BA6BB40C55815B7AF550D22A371EDD5E8BD87C65F537467874A37F0F538F5", /*3F*/
			"C23BBA2A9DECC0214E610E930B0E34501A681AA91477577E38A3209714EDC3760FD15563EEEB4AB67D57668A01D42178", /*40*/
			"6AF88CE145A098B51AEB858CD88B8B46E8B733AFB8E2D6E8313FAA8C10A7EBFA127D375E77557CEA96BDA2F70B2F2155", /*41*/
			"EC8903978FAFB636C7213C425C079763760384036AB6D17CE0C63A26385F1F49299877D6811A6DF5876F90FC5304B88D", /*42*/
			"A6FABBC2D6E0BA169B70C9640080E6BC29B2D5265598B27B4A9657C726E4397EA801CCC6766678D5800EF7CC72619998", /*43*/
			"235931A8CF5490BFE798F98E0E8F879FC6EEE29C38F30CA7929A79F2D53E002488F2E12749587A450B85B28F38891965", /*44*/
			"165E0303E4A4D82767994F42D1E8436AE6CC8BCF6E130D1B50101711709DDEFC373BDEC40CD0532840B274A4AA5109F6", /*45*/
			"A9F88BA008FDF8C8ECC897E3476EE05ABCE290AB69D57A74FA44DB1811E3115D6267AEFD64480C882697D04A2D3AECEB", /*46*/
			"C0782AF2ABCD331302BA1290F2F9627363C82F1A56ADC2B910F8E8C03EFE51C4E3EB348625CCAFFD93D607969CB8E7AE", /*47*/
			"CC6E179443E58FBCD21C93C655A7B8EE2B9834A31F2B8BA4C83B69516025ECEE9176EB7B427AAE948CB65B9E30B7A76E", /*48*/
			"C1A33A0AD6EDD98918B3C5D95813B5F7B024BD263B359A8BC8C17C2216A99B5071F9A11D582377293AA67C7618284290", /*49*/
			"99B7465E09201C7B9AF89FA01CA4FA81FC2EC63E761AD123E2A9A39585B17D1408394DE529F94E81479448E69794FAA4", /*4A*/
			"23CA3D1C4CBDCABBE3265436CE1A37E41BBF10F69E8A4CC905A66708048F5C4DE259DCDD9C5BFEFE439E65FAFD936EFD", /*4B*/
			"A24D73B6978F719C3F53F343CCB0BB8EBE3C72769EE07C6AFACB9E539CF558DD67B91D4E30DE986A1DB913D11698913A", /*4C*/
			"98BD4E140DC3C3C6142B1592BF3263E8CDBEAC59ED095B0E900763F0F625896AE213550F30324E398A13A4417A803195", /*4D*/
			"2ACD98ED8C6260731CAAA6B4C4CF323804DCB41EB677EB5DF88B5844A8105B68981D9E951A061A4DBC9471894C878EDB", /*4E*/
			"4959FEAD5D6C2DBD6ABD59E28C50304906D2C5494CAF8B3470E4541304A4293C520F3416CAF2F503B23D09D92613DB85", /*4F*/
			"26B5A815C32D17912C99E7555BB033C609CE9D6A0002514FD485282B2B8D79979C5B792F4A4A14C6851D9D02DC0BB4E7", /*50*/
			"62FEB6CACFB060EC9D977D69D5C661EABF08EFD806D8155625F1EEA460EA5718A25346B51F5A9665D92F9ADC358CA274", /*51*/
			"27E63DFC63E8FFA6CDB9CCE2CE99FDA3979D5B754974830D3298C8407D6693BE629D5FADA39B42B72654D31271CD84E1", /*52*/
			"AB1FA4DAF66E583CEEB6B7A236D24766A90738CDFDF5C6B328CBA9E5648E2D4CFDE5BF6C0CFE0DA39D00B863D7D78485", /*53*/
			"75FBBF094EEA16AA48931F027CD729F25D360679009B2E7FDDFCD148BD3DE21A4DBFF544B094D0E19C0E5C6294352C22", /*54*/
			"283A27FF968853D2B0960C6CEA0D03F2172BBA07A473DB38688C87D296E6F4BB5CB7E9BC5D68CF0F57A5D71B0E47BFB4", /*55*/
			"DE0108AAC1E4FF2FD346CFABEAC62B99B72E203F98B5F60881853D8CA54B29BEA6AED7C89FAA1680D2093B155C39D7ED", /*56*/
			"0BAEAC99D4974B84C7F258A699C9B4DA6F622C5E4ACCF5C158AB397D9781BEAABF811F67E101FFE3AFBCC2881C3C0EF3", /*57*/
			"26B211FB518D6C3E64BADAD51A10784AE6BE4E06A587186CD471F5C61343CD5C8389BB0DD6AAED5DC88112678914A17D", /*58*/
			"2B2D0BC3BB88D27DC5A7D1FAFF517AD296F39056A09F82ADFB38A61A6CED4D4E9D308E4EA6F9B2649097CE294AECC6B3", /*59*/
			"8FCA2B950690B1A2293EFCBF03D422DF8C9125B3E76353AB3D402092A1A701739BAB974CAB9BF6765EA8FCC55D8C586E", /*5A*/
			"408C92E8C2E1EC8C4AF4C914B71B43505186AEE0CDFB10692385EAFAB9657C67F708E4D3C898CA801EC8B9F89884907E", /*5B*/
			"46E8958B6A2C18782172FD410F78A6479D8E9DD83A299004390913C3265AD025D231F1E23077CBF1E7EE3E574E80D7F3", /*5C*/
			"5A8567A3D85E40B216ECF161133FCF7352DA5C6FBA3C0DD756E57983DEB34BFB83254FDCB768D1539A14F95F35C6B82D", /*5D*/
			"498A29C6E19D4AE62EF4AAF46027BA11BDBA7DAA84F39505940B2A04F6DC944D4E7ED35610FC0D53BADD94C2907E59E1", /*5E*/
			"14DF0FC43F475F8017E2AA8D264BF82F92625BDFE58B934D8384F415A4ACEA818E9C5EAEC5D8642B4D8EF55F1C826687", /*5F*/
			"4A2335C4F77128D9544E1476D29ABA94654EC86321785044B04AD9B02F80445AB0E01B6480C8D020596E325E88A3CBBF", /*60*/
			"896955157448D0620DB08C4C0F236D683BA8FC5B3CD1C4A204F57C53E144535BB7D04DCC7BE468404BBE993192334646", /*61*/
			"1D7837E6AB02CE273EA35BAED4493EA4D1CAFDB5DF94FABE98B580BB62170C4FC3C57A6CA9421C4368D65FC2C1201634", /*62*/
			"FAEABABC48717536454251E8F62F7315B318E8A7FDCDC5237C2E832013C913444D9E5DAFD169905212262E8C870537A7", /*63*/
			"8A3E5D0BEF8402A2A33BC5FAFA01990963CBE8ACD00762F5EA26A3F1819841786EEB78D1BB4AF6BB7ECF9671300E845F", /*64*/
			"0811B67CCCF5D0FC9F8CAB3F3496BD6B57CB7D24F1355C2D58218594165BDE80FAF3368A653A78F8C04CD80176267762", /*65*/
			"E6417CE75AAA23B034A7BFE3CBA617618C13E396F8C9B6ED5C9066464B09ED6376CB6A642C5CE283498E082A3EB449C6", /*66*/
			"6F2ADEA6357B5AA054DA382B15557B69302BD81946237AAE8F0CBB82111EFEDC45DD2DADCE20F2D38A77A5E9E8A2D1D8", /*67*/
			"E1EC332735862A2892B68B1A7E9C7C44F45618DC99E963E37CAC984502DD1A73C8650598CD70840D9A5DA584A26D4EFD", /*68*/
			"16B19B010740C15CB4544AC01016439A221F749C9E2F99A5A63E8A279A65570FC7231669ADD072ADC5BC35BA740BC801", /*69*/
			"6C44E75A4F378694D27ACE108A57764717C487FAFA7E15D66A3654D5C8E29EDF0CE35EEDCC611FFAD88A8C03C0095093", /*6A*/
			"CF106948BC4B1F2C91C0DC9990B99712193B21E3E109AB323340DE0608DD16668A5BB677BF602828402C410B1197B771", /*6B*/
			"EB080FF49CA5543EB4B9429542D6CA275999D45DC1533205F7EA9E398A1BEF3EBE8817775476DEC617064D7790C84100", /*6C*/
			"F3328E9150A7F8D652E3E61B04ACFDF851D82010F3CEB01559D673336676D5D84CB3BCEF1D91C3420C589AB58033BE49", /*6D*/
			"54B8E70EDCE038557BB590E99687FD576CFF08688D2B1FDDFD0F6D068BFE994FEB9BCE302489AE4466B21F200661E3E4", /*6E*/
			"2F5E0060189669AD473AF1D03C00CAE40278299268D1F3B4888714BC3A7EC9D29FF9C7F071EBD2D9875A5DC25DFFDB10", /*6F*/
			"E2A97A3E468399D83BF7EACA32C80DA113DCAC8EB6C2231D227EC90E1102EE97B2344832F03814348613888303B190EB", /*70*/
			"3A3D3B6CE026BFFE18D4953B9A68ED5924BB7B574AB777A0E0CB7DD64983DCB1CF768C439869AC978062BC7A900E6033", /*71*/
			"39D4C3B78A7A33C743D72EF22AB0B4EB54AE8184DDA503940C2A7DA083C385369DBC6F921D4AD8222CBB61FE182EAA42", /*72*/
			"D8CE9A806C0BD24DF69D65A65845727CC3FF81CC76F2B04876B1FDC3CA67CE58CED0970AFBCBE78A57502941B726F5F3", /*73*/
			"E006AEC17FCEFCF905CAA1629E003591B7050CC99F585312669260401E1594908442D25AA757CC5A228655CD4038770C", /*74*/
			"93EE8D67D3F1F3A1BEA46D48DBF8D7F43C91F02B8646453C6C3D7C1F04188A58EFA97287F89CEF84CB40364E108BFF4B", /*75*/
			"C6DCE3730D4FF82502AF54F87D9727907D69D20F6F4F788F90C255C64C166E8FA3529FBF4BF9C9A23ECEC41136694F6B", /*76*/
			"3DE10A5EC6CA7B3F7E196081D085ACAADF5F0DE3705D60F7393E7C83DCC57075A5F33BC2DCB98F970AEB7F050D1204C0", /*77*/
			"6F3B3B3D11A8BC05B52269AB2B95B8DC12EDE24EB1385F13202BBA6B5836B5E1EE3636C5925ACC4942224CF6EEB509BF", /*78*/
			"5F0CC3BBC4BE9A92584313FCCC54DD2EC11FE90F003940363371667C72FC97239611990B62AC8D9F4CFCB9EB3C317FAD", /*79*/
			"CA8E520A894A3FBABD9ED1B80098CC40BDF24507DFF3757C47AEC572E68D35ECF3D4523D27B373E41AB11E16973A05AB", /*7A*/
			"FFC293A6C26B817D2C9E9D134959D8287FA5216408199BBFA6F002DE0DCCD861BE8F9DC57F2CF35D1352E2DF86A47647", /*7B*/
			"84B55BE101708E743ADEC53721209F3EB18F9A1E68DFADBD09A050819774CF2DE4AB295D380A8762A3605B0C689C239F", /*7C*/
			"DDC7031FBFDFFE8F0B175DE65B832F0A31162ABC6571968551215E534BBC36B19F2F7D3B5D01AE44CF43A2426E83B61B", /*7D*/
			"7E32DB672B16F04ACE6F45DE0E6AB78825718548B8E70B41D7368BCF39A0FAC4956863EC49880C47720E335796341674", /*7E*/
			"06707A8E33D9D6C6B684BFE26CD576C644F47E5ECD5FC46CAF1B23A856D844B798A627916AC5657E040C3964A1127E19", /*7F*/
			"A5DAEC3134C0A39B0CA04160BD5ADB1FB50EC5A9F29E1ACBBE2FA1126AF7BFAFBEFC0AC4C9C5A4B3994739C71FB1EB29", /*80*/
			"6FEC2D343E83A7635BDBA5715757F50CD6F6282EE46A11B3A8B501F5922A5524A782A21006B605CAA10BD2E896975C81", /*81*/
			"B8AAE0532226D0ED891831C0470E84B774C824D648E8FF28B5E4E02EAD3906EB5ABB086ADA60A713A80C57666A9E29F1", /*82*/
			"529E3E52B1E7230A0C148861C9F08E260CFC8A131BAD803D8C09F324902FAA9F0231EE49879998483B0688492E2B5457", /*83*/
			"EFA6EAC5036814CD02773C1F8DAA5DF50E4EEDBD0702DE31BA7FD757D0D740EFA8805F0C74005F8B1448467BFF3E1EF8", /*84*/
			"2A07B766016AC70D64215C35364219E9CD6F7EFE35FCF6F1F05CC06084C29267AB3BF2F32579A444AC75F42D9A25B9C9", /*85*/
			"EF3A14B5EDDB84642314E0802D2DD0E914DEAEA9F928762A5763EBB480E15A0225F7CA14E8CDF5E68E594510DC61E6BC", /*86*/
			"E62C38DCFD21000B7BB32AE917EE3DA7E49F15E24CC9B65656E28259DCA361D8B43B8008A9285F480DC6B4AF7E4AE61B", /*87*/
			"703C64241142DCAE732D33342C45063A37877EA1624567CB2871D534614DD114E748092A1D94F5D14524056F0C6D1CB7", /*88*/
			"E325B1823A595DF9742D0DD5C96F397C44361C9540A9F45102382F9BF6331FB98ECBAFBBE91A0467528EBF3811F904A8", /*89*/
			"FD2BC6534631FB0D27A5F036FEEB9A6CD0F876D7911D077512EFB3A29C6E0B72DC4BCA3D5E871DA1028FB6E6E608F46F", /*8A*/
			"EF17ECC8930A7B4A9D97B34672FB273DC6AE835F35A25D8F6C27469530C21F5B2FBC16A26150E79502AD93AAE0B5C71A", /*8B*/
			"6D24BE43CF07DD5663681D62A38D2A2F9872C9B411724AA0B882B4857C19690A87B1BA8D2804C6F4D7B199CC36F40B49", /*8C*/
			"EEFB8D8573FD9E0F933403199B91560AFF0DB41665D5248C322EE105EA984196DB8CE0F83890D89B3A32F8983C901F80", /*8D*/
			"082CDAF93F215BAC67C118A1B9274FACAF74501CFB93198A53525CABA0E812D3C9AF3A005EFE8A6EF242DCB60DA7B2FE", /*8E*/
			"D3887FBFBB7314DFDDDCCCF0F720C342B2C4331C33C8C4151666010767F4785B8455B7C1FD5DE487A821C5EA181875F2", /*8F*/
			"7E289831418562F02AD12E3042B185C37C20D0D735A6AE96A68BEF98E22CBD41A1411D22F8D93243D813FB404F3D2F38", /*90*/
			"E13FC0A76F6642947E21C9D9F7FDDDCB161E68B366D6B1F855BF957EB5743874B23213EF8364D766529BB98AF96643D4", /*91*/
			"036D7ADDAADB5C330525835F802D032E7DF7D0D8D7A2BEF284927644B27696B7215E21E4D1F9B5B977743669C40EB7FD", /*92*/
			"A9B3534BE88977845BFD4283541A509097AFFCCD121C9778C146C4C9637989C70820E72FCBDA59C75526E2F4A0AE4F4F", /*93*/
			"A4739E20FD72BDC26D6EE5A5C1A54CA670A97A6FCB884E5C2B6108339E979C4893A63730D6BB23A75B1DCEAB00045EE5", /*94*/
			"427C14E4F88C8BDB1D8630868E039BC233DB40A251502D1BE043C9CCB45D2B3D292B67B6EE077B2D1C3A2FBDE24C742A", /*95*/
			"3DED69F37016D86A9A947B13AC66D7C3822D8645DF4CB39C2BA20F98F19E10DA6703138D422AC4C48D34D6138FA04A1D", /*96*/
			"28E59C8B257D112C8747068CC5499FCFD6C16EB780F9191AB416151633F7AF08A230E00D6BA1A1C3FD066FB9965B83D2", /*97*/
			"70F4BC1B7F8FFC3738DC0331E56B0FDCA9AB7290AD2B0BBDB307973C3D0783C6BDC455F6CDCA111F23F0E08317B8F0DC", /*98*/
			"0AEEC24E9285C50F3BCDA47833B61ACE839986F959EE0723C959034A8D7F5EB9D4AD7E05B05C4FB56C37A3D39F7A0EC4", /*99*/
			"0227B7230FBF2D0728D7D2AD632BED4707BD8F8B5012EFD048A0D43AE04034429B8939207F1449A1351EAD01B9FDF219", /*9A*/
			"A7119D2E311CEF251E532CD0C4ED04792272F878D8D30A0B769C412CED9C4C42262FFBFA65CBDDF5DB73D86721EA368E", /*9B*/
			"4BDBE90B3FBADCB21324EC3A8D6FEA576D9EFBE530850D00401A88AFF8A4C8F4655CB76B8A2E271C35505B6DBDE16F43", /*9C*/
			"6E15E57E23F570374962737362C1FA26C962372D1829B80BA1FE6832EA4D62116726E307F96E776304C761081677505B", /*9D*/
			"42E2FF3A8A6FC164FB85B2BC9D28B268C559CFF024533A282EC83F3911DAB3CEAE0FC74A9D736A27DB9CDD048BAB4CCF", /*9E*/
			"D79C52221D20E765499EDD73903CE7049B016D987DF48349FCFAB44AD12FC5C1811293F3B800FDF9511DC619CA53CEBE", /*9F*/
			"A059EE78B826EDDF4673AF294D17C85A5E527D4E4DF282B5DB5B9A2693F95CE36551D304FB54F296AB3EB70D65912FCC", /*A0*/
			"7D0C4F67B6C78135390CAEA7DE304D3749E19FABC8D494FE1A9E1B6437A04516886CC4BDAB6AF35A0529217344F502FE", /*A1*/
			"3CEDF34141B52CEE8133BA924753573FCB32BE22BC66025A0C480183DE403CB3BF5B84B427DFCF317251428DB0232156", /*A2*/
			"86FCE831C58E25CB5CC43FFE45CBFC7533877CC042F199BE1212FA7F0CC22E1C448EAB4B7D1F9823A7B1363A9FA7599E", /*A3*/
			"2D8C2FEDA0E5106D192E366838BBEB3F36226AA60ACEA0AFE7E1285DC1F3926A900371FA1883D9ECBAC33B1AF360EB66", /*A4*/
			"D4A2A11612BDE0E382AB0DA614CB4CB8189A4D50AC01F4C6E36A5DA1D9F6A647E43120D6B16B11B67D395F4236E75378", /*A5*/
			"C0C155CD47F3877F4B03BFE5C334CA7177710F1F4B844FF73443BBAB720E8DC5F03F8868C58634060FD60511C872EB50", /*A6*/
			"8C253DAAB52863069AA438F54A6196AC181D08C723A22C5E633C49C88E3910A1C9F54A67992675B01FDD98ACBD38D976", /*A7*/
			"A10893DA7575A9F78F5F4A025AB2A018D80538F0336BFFC00F9751D33889626F30383EB925BF911AE6149F68CE19CC60", /*A8*/
			"B9081DBAC6BE0598785DD9BC69C714928B035A0CA56E172B8946783500724888AF1E57C958650569E1DE4E944FF22261", /*A9*/
			"EA5EDC4D2718C0D2CB1C5D4DA15A8AE4C6272382F816301594A934E5057B54CE658E481A3D68D10DE8F24929E50A46A0", /*AA*/
			"7DF146281AF482CD014B68E726407B066CE564938C70DDBC36DAD2DE72A5DAA26D573BF69C0B2980684DAB14B4AA0329", /*AB*/
			"9C69DC064E738B5F83CC16BD5A1C36F5A99B365E6E141B122748FA5AD0FACCE826D073A047D99C49B005B182505B0C0C", /*AC*/
			"15B6A2A20ED0FD1C9333AF729BD65A2522CC333293BD2C1BD724D949B15E8BE169D0DB0512B9711785ACA8980DD7653C", /*AD*/
			"230EC629D77BB3F243115B991D297CB2A2F955792C53C76F48A76728EBE25BA77CE662A405384400DDC06B7E6BF49D66", /*AE*/
			"20DDB9BD7644410B056391B1FA2E8C06CA4EDE51CF167C0046602B550536F8705040672597C21FF40AF8EC6E8AFB844B", /*AF*/
			"0023C5749251B883335A4F86D66B7E00AE353DED3EFACE8F3FC80526D67B35DE0D9078FBDA80BC53467900DFF3FE4C14", /*B0*/
			"0F9CB2BE6A448113E38D541B6A9A5829673953DAF354FC0E3C818A277F8569E98D16EA77DB122A3BE40A860318B6EA84", /*B1*/
			"78CE11F42D7D5E5084F76DFF199C998D999B578E3AE935CBD9FD092C1BE6321231F33C63ACD316D85AA08030B8D65C0C", /*B2*/
			"0098DBE19CA84FE9E2426617D114213763C3C4166A78E21B74B145353E03B0E4F43C0824EAE508C458C1E6622528602A", /*B3*/
			"9E27EBE6D1426A6F2A6A600A6B5FA3428FF7E2306BA90370DF83D91A683EDDDD29572442F0225388E9CC0F1B6437320A", /*B4*/
			"054DF380E896064EFAB81A4AA3AD88A4F87426486CCA156FBB1B3C82374729607EC0B87CF73F960A5C57D7E6470F7808", /*B5*/
			"5758E103AC614A1A766AEE86F81358DF203FBA51DC74396A78C93DF969C5721FE69E32E230196597E287C6CECD8AB95B", /*B6*/
			"2A06A7C10C0DCC9799D5298268A6745FF2D818BB774858B3D52A820D4F64D8862F808EF87A263981BB91206E6347C676", /*B7*/
			"0847C6D71CE0C74686FD451B447C1E11C20623B0E2856FCC3ADDFA2D0398181E6736A0A06B336B46D1C70AEEB2B1257D", /*B8*/
			"5633260D141A9776D530805F596CA3DB8CE33EF69437CE46F62D54E97E747088DF5C9318489B45EAA4AAD29F0BA850CA", /*B9*/
			"BDBD7B16767F6D9FF7968427F1B7B6DD58C76599B35276EE286F4C7F6CADD7918188C0401742117BCEC4F1964266D163", /*BA*/
			"97E4E8A6B5135B248A8BD785E52979774545C1A0975BC5BB13FAE3BD9F59E37DAFD5627C0E91DE2BA223AC778474E1A9", /*BB*/
			"DE1BF1EAF86C6B3BA246A3ACD50035FE6F80179DD96A21CD3F8DB7CB17300D03497A798B5D94506CAD52DCC6F61AE841", /*BC*/
			"F4A4E1D08E1F440B5E27633CD56422E01465C14F1DB414209A939043988D37C2CBE65CFA245DB3686340AEDE28DDA855", /*BD*/
			"1F7AB65A3F892454D70AB4167EBEB5A19B2631E824C2028DD5D97BDEE31519BCEA2DC77449E4058CEB204F2D6D2FBAFF", /*BE*/
			"6537E69171A2665D3FD2F835435A3F23ADD5DD3E622D6C8AC522CDD5E5E243F85AEC27F3DBFDA8A2477A65ED570E1445", /*BF*/
			"3BA7CB01D32E9D639E335734E7B5416B0ED96A84F94539F645CEE2E46DF5A70DDE142EE1E9AFEC1C78D6121C4FDC72DD", /*C0*/
			"7BB30AF653390B772D394F2B7F8F7BB60277A3C213AF34897DF6E674DD56D0845643CD3073C42451FAB15F8BD1A1DC18", /*C1*/
			"42B453ABF5150D8B913F109C1188E18CC27BB7631FB43BF9EBDDE685EF10841976D67C87C56D33EA95EC73C0AF40F084", /*C2*/
			"BCE43D59A1F50BFBBA7027CA04D84600FB6FDB98A2BE644BD5DE777E993DED4AFCA39F1EDF710F3AA5E5893C858D8841", /*C3*/
			"C68AC776E6AEACFC538067C7866106EBD27B4A352F4EFDE3847DA2B3BF01E3783C79E3C136926D58F957BC8726AA1610", /*C4*/
			"95492C4203C7C6120DD60DB1EE8321FCE1D9EBA902F62B42EA2DBF7D0E37A4F2E11FB9098BF5DA48DBFE213F818EA338", /*C5*/
			"17CB21316D4756DDB88952498140146A648112F5808442884947ADC3F7D58F35651CCE28E26A53770B3803DAF337F89B", /*C6*/
			"BEAB16E2DCE6B6E38F39ECC8E39172DF607CC9553FF29C0E4BFD15154F4F0BA7EE6230B6BD408CE435B654110D164E99", /*C7*/
			"ADDDFF1BD2C11CD42A1A262CBA6E1AA00BF2291D09475A464C93A0ABADF4DE3273EE8E1327333E63F3AE2031F5D13B28", /*C8*/
			"246C7CABB2D9A55C50E9C7282C1EE0F62FBDA09565A0D3D7196552679C04A4EB137C66DA29A6DD8208A76B6B4BDA56BF", /*C9*/
			"7CA3C59BE3E286106ADD75CF1F7AE24801747450737A6435A1F2259CB2B4923BE0C8F55E8ECE7210D7964398F350B69B", /*CA*/
			"E045864ED1825101AC54969193E1A1C523D85A934D0794C7B4FA88CB734A42137C5CBFD6BDA3D5F966607FE938748825", /*CB*/
			"BAF36FD2A180D481EAC440AC1B9598F79AA24D80FFB7B06C79601F517358F163D1071831418BB63B819609A6AE7D3A03", /*CC*/
			"3E9152D8CDBAE55186AD793F203DD016BE3AEB778AD4A8912810254DD76B66189B5DCDE36636C3270A8AAD65868BC58C", /*CD*/
			"6D0672780D93152AEEE705247B8280919EBDB976F137463FA7DE3E73A2D0C1BFF871A00BA0046AC7484C96A803F23486", /*CE*/
			"0FC7BCDABB06BFFBF75C3FFB3D6309B3ECA305D103109162373F503B204FFF61CE332C9F54963FA29A4420A52242CDB4", /*CF*/
			"C71D481179D198C1505A2845CEE92569F339BFF6DD6755B58BEAD52B8DE892454B686E65920DCA2B99593FA43EE68A37", /*D0*/
			"D90A68D717E615019BB920AEA19161A62F3D6F96D90EB1E4DF15ECBA10513D7DE6E5D539B4F01831C7D17A7528FECE36", /*D1*/
			"A04FF0BEB4EBFBAFE5E90A5B3DDAA3CA8453542209F4A14580A6FFD72BB5A70714E0C4705A1ABF6AD699EC1FC18A677D", /*D2*/
			"7021A124E3181575DC7AAE2817AD945F8BB5521E7F0D565A6671D3792F0805EED3888EA394413A1ACE4D7E47B55BF9CC", /*D3*/
			"22F440263CAADE68E77BB287772EAC7B29493775962A40E91E06A27FA68CB91BDDEF02932ABDB9C779F03B88DC175233", /*D4*/
			"65F6D517B53E239197DB65A2F00B1C391D77AE9B85AA485519133B9B3E9B07716376D9F11A7DB3D4949AD02F5AE16184", /*D5*/
			"FE4434CDE09D923B03B0FCFD713B70522D713290D4A672382B56946FF629EE9660A15D01B2B3C4280B1D5EAF793933A0", /*D6*/
			"BC40FCFB0E0D494BA31C4648C7B3D1DEF1113C219A07EC8D2378BEB1A5C2BD1C190CC3478070A19463DAB6E1CCF56329", /*D7*/
			"901B6B9E82BABF91872A234C45D610016CA46A95C1CC6D6C22779315E0F0229560A59396346BE6ACFB67A503CB488846", /*D8*/
			"50D440F74C97660BE71ECABF64EDFE0C80201B895718CE22A05D89804D35D3068F700402A2B0D086326FCB334CA4DFC0", /*D9*/
			"BCFBD02EA005CDD5F0225A467555311508E18B3692A7AF6205D34A820C8CED0A51A8D7CEC33E80EA0AC007503FAE879C", /*DA*/
			"F43EEFB5C83C521AE5E9B05FC48841AC79C52C38BF85B5F926CD0818AE3BF7A94F385C32CA8F5F74F17B22107B954752", /*DB*/
			"1A48FC969198A4B0D9A78940BB0C4E1C42781D9BE60E769187D1CAF3680F8A30D09FF193606AAF294518DABC60048793", /*DC*/
			"F05D48134A56A03489A65EEB91DC69B98FC7F43960E63C62FA1C6B9FF9415E927E219D4E563479352B6A48D6DE0AEF85", /*DD*/
			"1A7FF9C54B045FFD44A0A9562E9468B2F11425A22D1EBF92208D33120BD28E0EF2D74197AF80E162CEEDCA73DFE66C93", /*DE*/
			"D57190439D29C9A444C007DC2B5EAF9DEF6DDF48A780CEDC61B205E4A96024B11885B6CE84C3FE5DB8B56986B6E2CE21", /*DF*/
			"F36DACFA34237E99BE45EB5253BCFED0402C6946B8B21AC02460A6FCE7E9CD67F89A6D5B162629FCF66CCEA374DB821E", /*E0*/
			"16E06074DCC31A1DF172017AC3FA38C3BBC1CE4BB784ED60DA89A8BCE82AE671A6DACFFB8D26C0BB185181AE9609F6D6", /*E1*/
			"F110DBDD94D17661F59FBB4CBA69F393463B60FB3F3C5E001C60B896FE8E78AC5EB3E26795DE5AB6997328D4654D6219", /*E2*/
			"21069118ABE24B61811CB8C48FCEFC6A483B032CFB56F902FB32E848198CC057A620815462A04F70900038D1894959E2", /*E3*/
			"5AD509789BFFECD0DDCD5E8325F69CA0154D8F1ACD9B8C82AC7DF75E94CE3CAF6D6554D1B38754BEB5DB64AF738486E7", /*E4*/
			"35A308A1AC9A43BF2647805AB3E6E4924BB74A616F61588FFA4602EE5BDBF54E3FDD62470A7174DB5795433CA808FAAC", /*E5*/
			"51A094B8774CA6055F07974C74EEF225022AFEF7AD81A9530967C44BBA336FD68AA327918AECBA3DF70B8436573C3F0A", /*E6*/
			"CF374F83420766C371F31901A13EF07C63AD56C7DEF9DC0F9E5BB5E859F5A231D0BF453BB9893E4CA1E14B66C2719760", /*E7*/
			"B41861CC73FD3E48461D79A138B04BE14010D37D37FBA8177D9622AA693225A42204454B8126799A33A5D487DCCD6EB6", /*E8*/
			"D291D0317A053320E27678F1E50D1F769A3D663A63159FC7AD7B4D3F67BAB452269CC05E2B33CE1C0FB8261CD734BCC3", /*E9*/
			"F3D0546D3D4A25EEB42874AD28C9B7F273EC788B29962D284AE73A48132B8553756C99D7A0910B66ECA7E2C2712D555C", /*EA*/
			"559FA5BF24911FDDA1DDF5DE3770554BC7C3FD139366B9466E7ECC0C881D2BA414E76D6A27E54B877352D5FBC4FAB878", /*EB*/
			"F19A622BED8DAC0A35548E5D7EFC5A2ECAC84974B4F057B2AB317ED03D0335AE710FC138F2C517389C90CC495A403416", /*EC*/
			"9FA7DEB936F10461A1529B0B58462F9D9F109111C8B9EC6523A3EB28444E33EA554084CA75118937599D58A7C946EAC2", /*ED*/
			"6EC3AABB7856AC4E980E6907C1CBCCAF1F8557ADC700CBF57DCB1CE0AF48D9F47FB3DADF8199AB8AE6B36DB8FADBF312", /*EE*/
			"C00F0D3F7A101660605B94B12DB6C69779944F7BA2B65F3840858ADEDD47E2BC1E044BDB0E9FB02B86C79D01A3109539", /*EF*/
			"9731893D5B98482AFB8DE267F97903268780F407143A505DA41CAEFCCCD3A8E3A042F0B3D7B7A7FE3E3151FEBB19A1AC", /*F0*/
			"E7EDF679003A6950BAFC97D4A8C6AB1213C096B49C79559AC3052501434B50191280FB23E7ADFB091959905D31BD2FC0", /*F1*/
			"575C0C46FCFCC65BFE625E873F34B4191696FDCC7F51B8A3C79C56F30E5AE7C014E3461CD27FAD151B7BCCB9CB472859", /*F2*/
			"3806FE58E5CC8F16F8244ED76734C1BF4E04940E0F5DDB565BD0AFDDC4158B7BA4C6BA949911C5C9FF6E2AC155AE9726", /*F3*/
			"49C7C844B8114144B450E41BCA35CB00302450EC67BEF97CA8662049DB1E0D8BDA69C022528EB8FA6ABBF16585C1A2F7", /*F4*/
			"37BB420DF67F044EDCC0E9F3E2EF07B34D100886187778410492E5379305DAAE3DA4791C37E4128F80688445CBA4EA17", /*F5*/
			"51398A7CE4CF8D9D49A5FCD891A69CA53D72A60EC2392DA50E8296B879AB55396BCB00AF2EDC0BDEBEB93848E54B3E90", /*F6*/
			"7AD7C52A18922E1929292C57C4F5B8F5F0CF1F98A577C10B072B9F293BB660CD09B8604F5575B6FBDECB396A81B9FCDB", /*F7*/
			"254AD7ADB4C220DE6C62E20F95A0070DADEB89F339309BD8A2F685CC178B289F9343905B5DEE95A5E0C30F34A2977C86", /*F8*/
			"669CD51AF7CFBFAAE3E0806F6880271D6934C259E098BF905DFEEAF0FBCA724989F74B948B4118B653640AEAFB6807C3", /*F9*/
			"DD3BACDCC04BE1206D4949BD64198E5131FDB39666598A74BBBC6DE9C0C15A81F27F201C61C062792738AFE3E84E5CDD", /*FA*/
			"CDD71FD35A6411DE3CC012793E87523FB0CFF8720FCA36F393E85FE07300F012E894A085263F090B2DF60A01DAFA90EC", /*FB*/
			"9DA50DB1EEB4FADDE524E49C9974799ADE09FFF26A24CBB9AF9D71E9F3ACE7CDEB62B1A62566EC9D06D02AB1217D3553", /*FC*/
			"DD31E6391AE0352293ACD1065B35E915F4EB56CC03E792180717815C850C97F1BF4F6A8AC0540A6FFCF8AE5DE9507FF0", /*FD*/
			"AB45B413DC50B20740B417369551D8D5CA32286A108E721003225E54D8D093AF4B6CA5591EA576E94E12AB774DC4E062", /*FE*/
			"D9F4F850DF6CB96C8ABAD81B1667335DCB4079CFE79C72E5E5542F763E316996303E4B79B9D397C4E46933038B945111", /*FF*/
			"75B15CC53B0D2502DA1BCA6BA05243589EDA977556C06B7E6C57727ECF0A1325DC613D5A78E5C3F8CE062D94A3B4945A"  /*00*/
	));
	
	//=========================================================================
	// INITIALIZATION
	//=========================================================================

	private static final List<String> ENCODER;
	
	static {
		final String[] escape = new String[256];
		for(int i = 0; i < escape.length; ++i) {
			final String escapeSequence;
			switch(i) {
			case 0x0A:
				escapeSequence = "\\n";
				break;
			case 0x0D:
				escapeSequence = "\\r";
				break;
			case 0x22:
				escapeSequence = "\\\"";
				break;
			case 0x5C:
				escapeSequence = "\\\\";
				break;
			default:
				if ((i <= 0x20) || (i >= 0x7F)) {
					escapeSequence = String.format("\\u%04X", i);
				}
				else {
					escapeSequence = String.valueOf((char)i);
				}
			}
			escape[i] = escapeSequence;
		}
		ENCODER = Collections.unmodifiableList(Arrays.asList(escape));
	}
	
	//=========================================================================
	// UTILITIES
	//=========================================================================

	private static byte[] decodeHexStr(final String hexStr) {
		final int len = hexStr.length() / 2;
		final byte[] result = new byte[len];
		int offset = 0;
		for(int i = 0; i < len; ++i) {
			final String current = hexStr.substring(offset, (offset += 2));
			result[i] = (byte)Integer.parseUnsignedInt(current, 16);
		}
		return result;
	}
	
	private static String bytesToString(final byte[] data) {
		final StringBuilder sb = new StringBuilder(); 
		for(final byte b : data) {
			sb.append(ENCODER.get(b & 0xFF));
		}
		return sb.toString();
	}
	
	//=========================================================================
	// MAIN FUNCTION
	//=========================================================================

	public static void main(String[] args) throws Exception {
		System.out.println("{");
		int index = 0;
		boolean first = true;
		for(final String hexStr : INPUT) {
			final byte[] bytes = decodeHexStr(hexStr);
			if(!first) {
				System.out.println(",");
			}
			else {
				first = false;
			}
			if(INPUT.size() > 257) {
				System.out.printf("\t/*%03d*/ \"%s\"", index++, bytesToString(bytes));
			}
			else {
				System.out.printf("\t/*%02X*/ \"%s\"", index++ % 256, bytesToString(bytes));
			}
		}
		System.out.println("\n}");
	}

}
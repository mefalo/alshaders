#include "alUtil.h"

void alphaInversion( const AtRGB& scatterColour, float scatterDist, AtRGB& sigma_s_prime_,
                                                                             AtRGB& sigma_a_ )
{
       AtRGB col = scatterColour;

       col = col / maxh(col);

       AtRGB alpha_prime = rgb(alpha1_3(clamp(col.r*.439f, 0.f, 1.f)),
                                   alpha1_3(clamp(col.g*.439f, 0.f, 1.f)),
                                   alpha1_3(clamp(col.b*.439f, 0.f, 1.f )) );

       AtRGB sigma_tr = rgb(1.0f,1.0f,1.0f) / (scatterColour * col);


       AtRGB sigma_t_prime = rgb(  sigma_tr[0] / ( sqrt( 3 * ( 1 - alpha_prime[0] ) ) ),
                                   sigma_tr[1] / ( sqrt( 3 * ( 1 - alpha_prime[1] ) ) ),
                                   sigma_tr[2] / ( sqrt( 3 * ( 1 - alpha_prime[2] ) ) ) );

       sigma_s_prime_ = sigma_t_prime * alpha_prime;
       sigma_a_ = sigma_t_prime - sigma_s_prime_;
}

float alpha1_3( float x )
{
       /* Discrete looking up tables for alpha, the average is 0.877051679 */
       float alpha1_3[2001] = {
       0.00000000f,   0.00558923f,   0.01115797f,   0.01665007f,   0.02210481f,
       0.02750112f,   0.03284671f,   0.03814856f,   0.04338977f,   0.04859844f,
       0.05373995f,   0.05885673f,   0.06390312f,   0.06892926f,   0.07388504f,
       0.07882177f,   0.08369136f,   0.08853990f,   0.09332764f,   0.09808919f,
       0.10279935f,   0.10747508f,   0.11211185f,   0.11670291f,   0.12127043f,
       0.12577795f,   0.13028026f,   0.13470538f,   0.13913050f,   0.14349029f,
       0.14783410f,   0.15213769f,   0.15640127f,   0.16065252f,   0.16483694f,
       0.16902137f,   0.17314595f,   0.17725226f,   0.18133307f,   0.18536231f,
       0.18939154f,   0.19335619f,   0.19730936f,   0.20123855f,   0.20511665f,
       0.20899475f,   0.21281796f,   0.21662198f,   0.22041780f,   0.22414869f,
       0.22787959f,   0.23157933f,   0.23523805f,   0.23889677f,   0.24250574f,
       0.24609323f,   0.24968071f,   0.25320414f,   0.25672132f,   0.26023378f,
       0.26368154f,   0.26712930f,   0.27056560f,   0.27394484f,   0.27732408f,
       0.28068924f,   0.28400083f,   0.28731243f,   0.29061143f,   0.29385624f,
       0.29710105f,   0.30033883f,   0.30351770f,   0.30669657f,   0.30987544f,
       0.31299175f,   0.31610551f,   0.31921927f,   0.32228487f,   0.32533434f,
       0.32838381f,   0.33140345f,   0.33438944f,   0.33737543f,   0.34035383f,
       0.34327712f,   0.34620042f,   0.34912372f,   0.35200366f,   0.35486504f,
       0.35772642f,   0.36057524f,   0.36337547f,   0.36617570f,   0.36897593f,
       0.37173785f,   0.37447768f,   0.37721751f,   0.37995734f,   0.38263844f,
       0.38531861f,   0.38799878f,   0.39066402f,   0.39328525f,   0.39590648f,
       0.39852771f,   0.40112343f,   0.40368644f,   0.40624945f,   0.40881246f,
       0.41134459f,   0.41385008f,   0.41635557f,   0.41886105f,   0.42133554f,
       0.42378419f,   0.42623284f,   0.42868149f,   0.43110422f,   0.43349670f,
       0.43588919f,   0.43828168f,   0.44065853f,   0.44299552f,   0.44533251f,
       0.44766950f,   0.45000634f,   0.45228848f,   0.45457062f,   0.45685277f,
       0.45913491f,   0.46138339f,   0.46361132f,   0.46583925f,   0.46806719f,
       0.47028802f,   0.47246237f,   0.47463672f,   0.47681107f,   0.47898542f,
       0.48113151f,   0.48325289f,   0.48537427f,   0.48749565f,   0.48961703f,
       0.49169550f,   0.49376451f,   0.49583352f,   0.49790253f,   0.49997155f,
       0.50198950f,   0.50400673f,   0.50602397f,   0.50804120f,   0.51005696f,
       0.51202300f,   0.51398903f,   0.51595507f,   0.51792111f,   0.51988715f,
       0.52180546f,   0.52372087f,   0.52563628f,   0.52755168f,   0.52946709f,
       0.53134636f,   0.53321169f,   0.53507702f,   0.53694235f,   0.53880768f,
       0.54065514f,   0.54247094f,   0.54428674f,   0.54610254f,   0.54791834f,
       0.54973414f,   0.55150812f,   0.55327492f,   0.55504172f,   0.55680852f,
       0.55857532f,   0.56033273f,   0.56205105f,   0.56376937f,   0.56548769f,
       0.56720601f,   0.56892433f,   0.57062471f,   0.57229506f,   0.57396541f,
       0.57563576f,   0.57730611f,   0.57897646f,   0.58062843f,   0.58225130f,
       0.58387418f,   0.58549706f,   0.58711994f,   0.58874281f,   0.59035510f,
       0.59193100f,   0.59350689f,   0.59508278f,   0.59665867f,   0.59823456f,
       0.59981045f,   0.60134542f,   0.60287480f,   0.60440418f,   0.60593356f,
       0.60746294f,   0.60899232f,   0.61050599f,   0.61198933f,   0.61347266f,
       0.61495599f,   0.61643932f,   0.61792265f,   0.61940599f,   0.62086198f,
       0.62229972f,   0.62373746f,   0.62517519f,   0.62661293f,   0.62805067f,
       0.62948840f,   0.63089705f,   0.63228964f,   0.63368222f,   0.63507481f,
       0.63646739f,   0.63785997f,   0.63925256f,   0.64062442f,   0.64197228f,
       0.64332014f,   0.64466800f,   0.64601586f,   0.64736372f,   0.64871158f,
       0.65005749f,   0.65136105f,   0.65266461f,   0.65396816f,   0.65527172f,
       0.65657528f,   0.65787883f,   0.65918239f,   0.66046959f,   0.66172925f,
       0.66298891f,   0.66424858f,   0.66550824f,   0.66676790f,   0.66802756f,
       0.66928723f,   0.67052801f,   0.67174417f,   0.67296034f,   0.67417651f,
       0.67539268f,   0.67660884f,   0.67782501f,   0.67904118f,   0.68024822f,
       0.68142128f,   0.68259434f,   0.68376740f,   0.68494046f,   0.68611352f,
       0.68728658f,   0.68845964f,   0.68963270f,   0.69077641f,   0.69190674f,
       0.69303707f,   0.69416740f,   0.69529773f,   0.69642806f,   0.69755839f,
       0.69868872f,   0.69981905f,   0.70091380f,   0.70200177f,   0.70308974f,
       0.70417771f,   0.70526568f,   0.70635365f,   0.70744162f,   0.70852959f,
       0.70961756f,   0.71067829f,   0.71172426f,   0.71277022f,   0.71381619f,
       0.71486216f,   0.71590812f,   0.71695409f,   0.71800006f,   0.71904602f,
       0.72008833f,   0.72109264f,   0.72209696f,   0.72310127f,   0.72410559f,
       0.72510990f,   0.72611422f,   0.72711853f,   0.72812285f,   0.72912716f,
       0.73012607f,   0.73108907f,   0.73205207f,   0.73301508f,   0.73397808f,
       0.73494109f,   0.73590409f,   0.73686709f,   0.73783010f,   0.73879310f,
       0.73975611f,   0.74068851f,   0.74161054f,   0.74253256f,   0.74345459f,
       0.74437662f,   0.74529864f,   0.74622067f,   0.74714270f,   0.74806473f,
       0.74898675f,   0.74990878f,   0.75079418f,   0.75167555f,   0.75255693f,
       0.75343831f,   0.75431968f,   0.75520106f,   0.75608243f,   0.75696381f,
       0.75784519f,   0.75872656f,   0.75960794f,   0.76046693f,   0.76130797f,
       0.76214902f,   0.76299006f,   0.76383111f,   0.76467216f,   0.76551320f,
       0.76635425f,   0.76719530f,   0.76803634f,   0.76887739f,   0.76971843f,
       0.77053286f,   0.77133389f,   0.77213492f,   0.77293595f,   0.77373699f,
       0.77453802f,   0.77533905f,   0.77614008f,   0.77694111f,   0.77774214f,
       0.77854317f,   0.77934420f,   0.78013804f,   0.78089936f,   0.78166069f,
       0.78242202f,   0.78318335f,   0.78394467f,   0.78470600f,   0.78546733f,
       0.78622866f,   0.78698998f,   0.78775131f,   0.78851264f,   0.78927397f,
       0.79003347f,   0.79075540f,   0.79147733f,   0.79219927f,   0.79292120f,
       0.79364313f,   0.79436506f,   0.79508700f,   0.79580893f,   0.79653086f,
       0.79725279f,   0.79797473f,   0.79869666f,   0.79941859f,   0.80013292f,
       0.80081576f,   0.80149861f,   0.80218145f,   0.80286430f,   0.80354714f,
       0.80422999f,   0.80491284f,   0.80559568f,   0.80627853f,   0.80696137f,
       0.80764422f,   0.80832706f,   0.80900991f,   0.80969276f,   0.81035427f,
       0.81099834f,   0.81164241f,   0.81228648f,   0.81293055f,   0.81357461f,
       0.81421868f,   0.81486275f,   0.81550682f,   0.81615089f,   0.81679495f,
       0.81743902f,   0.81808309f,   0.81872716f,   0.81937123f,   0.82001438f,
       0.82061999f,   0.82122559f,   0.82183119f,   0.82243680f,   0.82304240f,
       0.82364800f,   0.82425361f,   0.82485921f,   0.82546481f,   0.82607042f,
       0.82667602f,   0.82728162f,   0.82788723f,   0.82849283f,   0.82909844f,
       0.82970404f,   0.83029014f,   0.83085760f,   0.83142506f,   0.83199251f,
       0.83255997f,   0.83312743f,   0.83369489f,   0.83426235f,   0.83482981f,
       0.83539727f,   0.83596472f,   0.83653218f,   0.83709964f,   0.83766710f,
       0.83823456f,   0.83880202f,   0.83936947f,   0.83993693f,   0.84047078f,
       0.84100042f,   0.84153006f,   0.84205970f,   0.84258935f,   0.84311899f,
       0.84364863f,   0.84417827f,   0.84470792f,   0.84523756f,   0.84576720f,
       0.84629684f,   0.84682648f,   0.84735613f,   0.84788577f,   0.84841541f,
       0.84894505f,   0.84947470f,   0.85000403f,   0.85049620f,   0.85098837f,
       0.85148054f,   0.85197271f,   0.85246487f,   0.85295704f,   0.85344921f,
       0.85394138f,   0.85443355f,   0.85492572f,   0.85541789f,   0.85591006f,
       0.85640222f,   0.85689439f,   0.85738656f,   0.85787873f,   0.85837090f,
       0.85886307f,   0.85935524f,   0.85984740f,   0.86031397f,   0.86076902f,
       0.86122408f,   0.86167913f,   0.86213419f,   0.86258924f,   0.86304430f,
       0.86349935f,   0.86395441f,   0.86440946f,   0.86486452f,   0.86531957f,
       0.86577463f,   0.86622969f,   0.86668474f,   0.86713980f,   0.86759485f,
       0.86804991f,   0.86850496f,   0.86896002f,   0.86941507f,   0.86987013f,
       0.87029894f,   0.87071726f,   0.87113559f,   0.87155391f,   0.87197224f,
       0.87239056f,   0.87280889f,   0.87322721f,   0.87364554f,   0.87406386f,
       0.87448219f,   0.87490051f,   0.87531884f,   0.87573716f,   0.87615549f,
       0.87657381f,   0.87699214f,   0.87741046f,   0.87782879f,   0.87824711f,
       0.87866544f,   0.87908376f,   0.87950209f,   0.87992041f,   0.88030933f,
       0.88069134f,   0.88107334f,   0.88145535f,   0.88183735f,   0.88221936f,
       0.88260137f,   0.88298337f,   0.88336538f,   0.88374739f,   0.88412939f,
       0.88451140f,   0.88489341f,   0.88527541f,   0.88565742f,   0.88603943f,
       0.88642143f,   0.88680344f,   0.88718545f,   0.88756745f,   0.88794946f,
       0.88833146f,   0.88871347f,   0.88909548f,   0.88947748f,   0.88985949f,
       0.89021882f,   0.89056495f,   0.89091109f,   0.89125722f,   0.89160336f,
       0.89194949f,   0.89229563f,   0.89264176f,   0.89298789f,   0.89333403f,
       0.89368016f,   0.89402630f,   0.89437243f,   0.89471857f,   0.89506470f,
       0.89541084f,   0.89575697f,   0.89610310f,   0.89644924f,   0.89679537f,
       0.89714151f,   0.89748764f,   0.89783378f,   0.89817991f,   0.89852605f,
       0.89887218f,   0.89921831f,   0.89956445f,   0.89991058f,   0.90023048f,
       0.90054123f,   0.90085198f,   0.90116273f,   0.90147348f,   0.90178423f,
       0.90209498f,   0.90240573f,   0.90271648f,   0.90302724f,   0.90333799f,
       0.90364874f,   0.90395949f,   0.90427024f,   0.90458099f,   0.90489174f,
       0.90520249f,   0.90551324f,   0.90582400f,   0.90613475f,   0.90644550f,
       0.90675625f,   0.90706700f,   0.90737775f,   0.90768850f,   0.90799925f,
       0.90831000f,   0.90862076f,   0.90893151f,   0.90924226f,   0.90955301f,
       0.90986376f,   0.91015494f,   0.91043085f,   0.91070676f,   0.91098266f,
       0.91125857f,   0.91153447f,   0.91181038f,   0.91208629f,   0.91236219f,
       0.91263810f,   0.91291401f,   0.91318991f,   0.91346582f,   0.91374173f,
       0.91401763f,   0.91429354f,   0.91456945f,   0.91484535f,   0.91512126f,
       0.91539716f,   0.91567307f,   0.91594898f,   0.91622488f,   0.91650079f,
       0.91677670f,   0.91705260f,   0.91732851f,   0.91760442f,   0.91788032f,
       0.91815623f,   0.91843213f,   0.91870804f,   0.91898395f,   0.91925985f,
       0.91953576f,   0.91981167f,   0.92007670f,   0.92031836f,   0.92056002f,
       0.92080168f,   0.92104333f,   0.92128499f,   0.92152665f,   0.92176831f,
       0.92200997f,   0.92225162f,   0.92249328f,   0.92273494f,   0.92297660f,
       0.92321826f,   0.92345992f,   0.92370157f,   0.92394323f,   0.92418489f,
       0.92442655f,   0.92466821f,   0.92490986f,   0.92515152f,   0.92539318f,
       0.92563484f,   0.92587650f,   0.92611815f,   0.92635981f,   0.92660147f,
       0.92684313f,   0.92708479f,   0.92732644f,   0.92756810f,   0.92780976f,
       0.92805142f,   0.92829308f,   0.92853473f,   0.92877639f,   0.92901805f,
       0.92925971f,   0.92950137f,   0.92974302f,   0.92998468f,   0.93019488f,
       0.93040295f,   0.93061103f,   0.93081910f,   0.93102717f,   0.93123524f,
       0.93144331f,   0.93165138f,   0.93185945f,   0.93206752f,   0.93227560f,
       0.93248367f,   0.93269174f,   0.93289981f,   0.93310788f,   0.93331595f,
       0.93352402f,   0.93373209f,   0.93394017f,   0.93414824f,   0.93435631f,
       0.93456438f,   0.93477245f,   0.93498052f,   0.93518859f,   0.93539667f,
       0.93560474f,   0.93581281f,   0.93602088f,   0.93622895f,   0.93643702f,
       0.93664509f,   0.93685316f,   0.93706124f,   0.93726931f,   0.93747738f,
       0.93768545f,   0.93789352f,   0.93810159f,   0.93830966f,   0.93851773f,
       0.93872581f,   0.93893388f,   0.93914195f,   0.93935002f,   0.93955809f,
       0.93976616f,   0.93997423f,   0.94015352f,   0.94032873f,   0.94050395f,
       0.94067917f,   0.94085438f,   0.94102960f,   0.94120482f,   0.94138003f,
       0.94155525f,   0.94173046f,   0.94190568f,   0.94208090f,   0.94225611f,
       0.94243133f,   0.94260655f,   0.94278176f,   0.94295698f,   0.94313219f,
       0.94330741f,   0.94348263f,   0.94365784f,   0.94383306f,   0.94400827f,
       0.94418349f,   0.94435871f,   0.94453392f,   0.94470914f,   0.94488436f,
       0.94505957f,   0.94523479f,   0.94541000f,   0.94558522f,   0.94576044f,
       0.94593565f,   0.94611087f,   0.94628608f,   0.94646130f,   0.94663652f,
       0.94681173f,   0.94698695f,   0.94716217f,   0.94733738f,   0.94751260f,
       0.94768781f,   0.94786303f,   0.94803825f,   0.94821346f,   0.94838868f,
       0.94856389f,   0.94873911f,   0.94891433f,   0.94908954f,   0.94926476f,
       0.94943998f,   0.94961519f,   0.94979041f,   0.94996562f,   0.95011507f,
       0.95025823f,   0.95040139f,   0.95054455f,   0.95068771f,   0.95083088f,
       0.95097404f,   0.95111720f,   0.95126036f,   0.95140352f,   0.95154668f,
       0.95168984f,   0.95183300f,   0.95197616f,   0.95211932f,   0.95226248f,
       0.95240564f,   0.95254880f,   0.95269196f,   0.95283512f,   0.95297828f,
       0.95312144f,   0.95326460f,   0.95340776f,   0.95355092f,   0.95369408f,
       0.95383724f,   0.95398040f,   0.95412356f,   0.95426672f,   0.95440988f,
       0.95455304f,   0.95469620f,   0.95483936f,   0.95498252f,   0.95512568f,
       0.95526885f,   0.95541201f,   0.95555517f,   0.95569833f,   0.95584149f,
       0.95598465f,   0.95612781f,   0.95627097f,   0.95641413f,   0.95655729f,
       0.95670045f,   0.95684361f,   0.95698677f,   0.95712993f,   0.95727309f,
       0.95741625f,   0.95755941f,   0.95770257f,   0.95784573f,   0.95798889f,
       0.95813205f,   0.95827521f,   0.95841837f,   0.95856153f,   0.95870469f,
       0.95884785f,   0.95899101f,   0.95913417f,   0.95927733f,   0.95942049f,
       0.95956365f,   0.95970682f,   0.95984998f,   0.95999314f,   0.96010658f,
       0.96021853f,   0.96033048f,   0.96044243f,   0.96055438f,   0.96066633f,
       0.96077828f,   0.96089022f,   0.96100217f,   0.96111412f,   0.96122607f,
       0.96133802f,   0.96144997f,   0.96156192f,   0.96167387f,   0.96178582f,
       0.96189777f,   0.96200971f,   0.96212166f,   0.96223361f,   0.96234556f,
       0.96245751f,   0.96256946f,   0.96268141f,   0.96279336f,   0.96290531f,
       0.96301726f,   0.96312920f,   0.96324115f,   0.96335310f,   0.96346505f,
       0.96357700f,   0.96368895f,   0.96380090f,   0.96391285f,   0.96402480f,
       0.96413675f,   0.96424869f,   0.96436064f,   0.96447259f,   0.96458454f,
       0.96469649f,   0.96480844f,   0.96492039f,   0.96503234f,   0.96514429f,
       0.96525624f,   0.96536818f,   0.96548013f,   0.96559208f,   0.96570403f,
       0.96581598f,   0.96592793f,   0.96603988f,   0.96615183f,   0.96626378f,
       0.96637573f,   0.96648768f,   0.96659962f,   0.96671157f,   0.96682352f,
       0.96693547f,   0.96704742f,   0.96715937f,   0.96727132f,   0.96738327f,
       0.96749522f,   0.96760717f,   0.96771911f,   0.96783106f,   0.96794301f,
       0.96805496f,   0.96816691f,   0.96827886f,   0.96839081f,   0.96850276f,
       0.96861471f,   0.96872666f,   0.96883860f,   0.96895055f,   0.96906250f,
       0.96917445f,   0.96928640f,   0.96939835f,   0.96951030f,   0.96962225f,
       0.96973420f,   0.96984615f,   0.96995809f,   0.97005102f,   0.97013256f,
       0.97021410f,   0.97029564f,   0.97037719f,   0.97045873f,   0.97054027f,
       0.97062181f,   0.97070335f,   0.97078490f,   0.97086644f,   0.97094798f,
       0.97102952f,   0.97111106f,   0.97119261f,   0.97127415f,   0.97135569f,
       0.97143723f,   0.97151877f,   0.97160031f,   0.97168186f,   0.97176340f,
       0.97184494f,   0.97192648f,   0.97200802f,   0.97208957f,   0.97217111f,
       0.97225265f,   0.97233419f,   0.97241573f,   0.97249728f,   0.97257882f,
       0.97266036f,   0.97274190f,   0.97282344f,   0.97290499f,   0.97298653f,
       0.97306807f,   0.97314961f,   0.97323115f,   0.97331269f,   0.97339424f,
       0.97347578f,   0.97355732f,   0.97363886f,   0.97372040f,   0.97380195f,
       0.97388349f,   0.97396503f,   0.97404657f,   0.97412811f,   0.97420966f,
       0.97429120f,   0.97437274f,   0.97445428f,   0.97453582f,   0.97461737f,
       0.97469891f,   0.97478045f,   0.97486199f,   0.97494353f,   0.97502507f,
       0.97510662f,   0.97518816f,   0.97526970f,   0.97535124f,   0.97543278f,
       0.97551433f,   0.97559587f,   0.97567741f,   0.97575895f,   0.97584049f,
       0.97592204f,   0.97600358f,   0.97608512f,   0.97616666f,   0.97624820f,
       0.97632975f,   0.97641129f,   0.97649283f,   0.97657437f,   0.97665591f,
       0.97673746f,   0.97681900f,   0.97690054f,   0.97698208f,   0.97706362f,
       0.97714516f,   0.97722671f,   0.97730825f,   0.97738979f,   0.97747133f,
       0.97755287f,   0.97763442f,   0.97771596f,   0.97779750f,   0.97787904f,
       0.97796058f,   0.97804213f,   0.97812367f,   0.97820521f,   0.97828675f,
       0.97836829f,   0.97844984f,   0.97853138f,   0.97861292f,   0.97869446f,
       0.97877600f,   0.97885754f,   0.97893909f,   0.97902063f,   0.97910217f,
       0.97918371f,   0.97926525f,   0.97934680f,   0.97942834f,   0.97950988f,
       0.97959142f,   0.97967296f,   0.97975451f,   0.97983605f,   0.97991759f,
       0.97999913f,   0.98005099f,   0.98010252f,   0.98015406f,   0.98020559f,
       0.98025713f,   0.98030867f,   0.98036020f,   0.98041174f,   0.98046327f,
       0.98051481f,   0.98056634f,   0.98061788f,   0.98066941f,   0.98072095f,
       0.98077249f,   0.98082402f,   0.98087556f,   0.98092709f,   0.98097863f,
       0.98103016f,   0.98108170f,   0.98113324f,   0.98118477f,   0.98123631f,
       0.98128784f,   0.98133938f,   0.98139091f,   0.98144245f,   0.98149399f,
       0.98154552f,   0.98159706f,   0.98164859f,   0.98170013f,   0.98175166f,
       0.98180320f,   0.98185474f,   0.98190627f,   0.98195781f,   0.98200934f,
       0.98206088f,   0.98211241f,   0.98216395f,   0.98221549f,   0.98226702f,
       0.98231856f,   0.98237009f,   0.98242163f,   0.98247316f,   0.98252470f,
       0.98257623f,   0.98262777f,   0.98267931f,   0.98273084f,   0.98278238f,
       0.98283391f,   0.98288545f,   0.98293698f,   0.98298852f,   0.98304006f,
       0.98309159f,   0.98314313f,   0.98319466f,   0.98324620f,   0.98329773f,
       0.98334927f,   0.98340081f,   0.98345234f,   0.98350388f,   0.98355541f,
       0.98360695f,   0.98365848f,   0.98371002f,   0.98376156f,   0.98381309f,
       0.98386463f,   0.98391616f,   0.98396770f,   0.98401923f,   0.98407077f,
       0.98412231f,   0.98417384f,   0.98422538f,   0.98427691f,   0.98432845f,
       0.98437998f,   0.98443152f,   0.98448306f,   0.98453459f,   0.98458613f,
       0.98463766f,   0.98468920f,   0.98474073f,   0.98479227f,   0.98484380f,
       0.98489534f,   0.98494688f,   0.98499841f,   0.98504995f,   0.98510148f,
       0.98515302f,   0.98520455f,   0.98525609f,   0.98530763f,   0.98535916f,
       0.98541070f,   0.98546223f,   0.98551377f,   0.98556530f,   0.98561684f,
       0.98566838f,   0.98571991f,   0.98577145f,   0.98582298f,   0.98587452f,
       0.98592605f,   0.98597759f,   0.98602913f,   0.98608066f,   0.98613220f,
       0.98618373f,   0.98623527f,   0.98628680f,   0.98633834f,   0.98638988f,
       0.98644141f,   0.98649295f,   0.98654448f,   0.98659602f,   0.98664755f,
       0.98669909f,   0.98675063f,   0.98680216f,   0.98685370f,   0.98690523f,
       0.98695677f,   0.98700830f,   0.98705984f,   0.98711137f,   0.98716291f,
       0.98721445f,   0.98726598f,   0.98731752f,   0.98736905f,   0.98742059f,
       0.98747212f,   0.98752366f,   0.98757520f,   0.98762673f,   0.98767827f,
       0.98772980f,   0.98778134f,   0.98783287f,   0.98788441f,   0.98793595f,
       0.98798748f,   0.98803902f,   0.98809055f,   0.98814209f,   0.98819362f,
       0.98824516f,   0.98829670f,   0.98834823f,   0.98839977f,   0.98845130f,
       0.98850284f,   0.98855437f,   0.98860591f,   0.98865745f,   0.98870898f,
       0.98876052f,   0.98881205f,   0.98886359f,   0.98891512f,   0.98896666f,
       0.98901820f,   0.98906973f,   0.98912127f,   0.98917280f,   0.98922434f,
       0.98927587f,   0.98932741f,   0.98937894f,   0.98943048f,   0.98948202f,
       0.98953355f,   0.98958509f,   0.98963662f,   0.98968816f,   0.98973969f,
       0.98979123f,   0.98984277f,   0.98989430f,   0.98994584f,   0.98999737f,
       0.99001335f,   0.99002741f,   0.99004148f,   0.99005555f,   0.99006961f,
       0.99008368f,   0.99009774f,   0.99011181f,   0.99012587f,   0.99013994f,
       0.99015401f,   0.99016807f,   0.99018214f,   0.99019620f,   0.99021027f,
       0.99022433f,   0.99023840f,   0.99025247f,   0.99026653f,   0.99028060f,
       0.99029466f,   0.99030873f,   0.99032279f,   0.99033686f,   0.99035093f,
       0.99036499f,   0.99037906f,   0.99039312f,   0.99040719f,   0.99042125f,
       0.99043532f,   0.99044939f,   0.99046345f,   0.99047752f,   0.99049158f,
       0.99050565f,   0.99051971f,   0.99053378f,   0.99054785f,   0.99056191f,
       0.99057598f,   0.99059004f,   0.99060411f,   0.99061817f,   0.99063224f,
       0.99064631f,   0.99066037f,   0.99067444f,   0.99068850f,   0.99070257f,
       0.99071663f,   0.99073070f,   0.99074477f,   0.99075883f,   0.99077290f,
       0.99078696f,   0.99080103f,   0.99081509f,   0.99082916f,   0.99084323f,
       0.99085729f,   0.99087136f,   0.99088542f,   0.99089949f,   0.99091355f,
       0.99092762f,   0.99094169f,   0.99095575f,   0.99096982f,   0.99098388f,
       0.99099795f,   0.99101201f,   0.99102608f,   0.99104015f,   0.99105421f,
       0.99106828f,   0.99108234f,   0.99109641f,   0.99111047f,   0.99112454f,
       0.99113861f,   0.99115267f,   0.99116674f,   0.99118080f,   0.99119487f,
       0.99120893f,   0.99122300f,   0.99123707f,   0.99125113f,   0.99126520f,
       0.99127926f,   0.99129333f,   0.99130739f,   0.99132146f,   0.99133553f,
       0.99134959f,   0.99136366f,   0.99137772f,   0.99139179f,   0.99140585f,
       0.99141992f,   0.99143399f,   0.99144805f,   0.99146212f,   0.99147618f,
       0.99149025f,   0.99150431f,   0.99151838f,   0.99153244f,   0.99154651f,
       0.99156058f,   0.99157464f,   0.99158871f,   0.99160277f,   0.99161684f,
       0.99163090f,   0.99164497f,   0.99165904f,   0.99167310f,   0.99168717f,
       0.99170123f,   0.99171530f,   0.99172936f,   0.99174343f,   0.99175750f,
       0.99177156f,   0.99178563f,   0.99179969f,   0.99181376f,   0.99182782f,
       0.99184189f,   0.99185596f,   0.99187002f,   0.99188409f,   0.99189815f,
       0.99191222f,   0.99192628f,   0.99194035f,   0.99195442f,   0.99196848f,
       0.99198255f,   0.99199661f,   0.99201068f,   0.99202474f,   0.99203881f,
       0.99205288f,   0.99206694f,   0.99208101f,   0.99209507f,   0.99210914f,
       0.99212320f,   0.99213727f,   0.99215134f,   0.99216540f,   0.99217947f,
       0.99219353f,   0.99220760f,   0.99222166f,   0.99223573f,   0.99224980f,
       0.99226386f,   0.99227793f,   0.99229199f,   0.99230606f,   0.99232012f,
       0.99233419f,   0.99234826f,   0.99236232f,   0.99237639f,   0.99239045f,
       0.99240452f,   0.99241858f,   0.99243265f,   0.99244672f,   0.99246078f,
       0.99247485f,   0.99248891f,   0.99250298f,   0.99251704f,   0.99253111f,
       0.99254518f,   0.99255924f,   0.99257331f,   0.99258737f,   0.99260144f,
       0.99261550f,   0.99262957f,   0.99264364f,   0.99265770f,   0.99267177f,
       0.99268583f,   0.99269990f,   0.99271396f,   0.99272803f,   0.99274210f,
       0.99275616f,   0.99277023f,   0.99278429f,   0.99279836f,   0.99281242f,
       0.99282649f,   0.99284056f,   0.99285462f,   0.99286869f,   0.99288275f,
       0.99289682f,   0.99291088f,   0.99292495f,   0.99293902f,   0.99295308f,
       0.99296715f,   0.99298121f,   0.99299528f,   0.99300934f,   0.99302341f,
       0.99303748f,   0.99305154f,   0.99306561f,   0.99307967f,   0.99309374f,
       0.99310780f,   0.99312187f,   0.99313594f,   0.99315000f,   0.99316407f,
       0.99317813f,   0.99319220f,   0.99320626f,   0.99322033f,   0.99323440f,
       0.99324846f,   0.99326253f,   0.99327659f,   0.99329066f,   0.99330472f,
       0.99331879f,   0.99333286f,   0.99334692f,   0.99336099f,   0.99337505f,
       0.99338912f,   0.99340318f,   0.99341725f,   0.99343132f,   0.99344538f,
       0.99345945f,   0.99347351f,   0.99348758f,   0.99350164f,   0.99351571f,
       0.99352978f,   0.99354384f,   0.99355791f,   0.99357197f,   0.99358604f,
       0.99360010f,   0.99361417f,   0.99362824f,   0.99364230f,   0.99365637f,
       0.99367043f,   0.99368450f,   0.99369856f,   0.99371263f,   0.99372670f,
       0.99374076f,   0.99375483f,   0.99376889f,   0.99378296f,   0.99379702f,
       0.99381109f,   0.99382516f,   0.99383922f,   0.99385329f,   0.99386735f,
       0.99388142f,   0.99389548f,   0.99390955f,   0.99392361f,   0.99393768f,
       0.99395175f,   0.99396581f,   0.99397988f,   0.99399394f,   0.99400801f,
       0.99402207f,   0.99403614f,   0.99405021f,   0.99406427f,   0.99407834f,
       0.99409240f,   0.99410647f,   0.99412053f,   0.99413460f,   0.99414867f,
       0.99416273f,   0.99417680f,   0.99419086f,   0.99420493f,   0.99421899f,
       0.99423306f,   0.99424713f,   0.99426119f,   0.99427526f,   0.99428932f,
       0.99430339f,   0.99431745f,   0.99433152f,   0.99434559f,   0.99435965f,
       0.99437372f,   0.99438778f,   0.99440185f,   0.99441591f,   0.99442998f,
       0.99444405f,   0.99445811f,   0.99447218f,   0.99448624f,   0.99450031f,
       0.99451437f,   0.99452844f,   0.99454251f,   0.99455657f,   0.99457064f,
       0.99458470f,   0.99459877f,   0.99461283f,   0.99462690f,   0.99464097f,
       0.99465503f,   0.99466910f,   0.99468316f,   0.99469723f,   0.99471129f,
       0.99472536f,   0.99473943f,   0.99475349f,   0.99476756f,   0.99478162f,
       0.99479569f,   0.99480975f,   0.99482382f,   0.99483789f,   0.99485195f,
       0.99486602f,   0.99488008f,   0.99489415f,   0.99490821f,   0.99492228f,
       0.99493635f,   0.99495041f,   0.99496448f,   0.99497854f,   0.99499261f,
       0.99500667f,   0.99502074f,   0.99503481f,   0.99504887f,   0.99506294f,
       0.99507700f,   0.99509107f,   0.99510513f,   0.99511920f,   0.99513327f,
       0.99514733f,   0.99516140f,   0.99517546f,   0.99518953f,   0.99520359f,
       0.99521766f,   0.99523173f,   0.99524579f,   0.99525986f,   0.99527392f,
       0.99528799f,   0.99530205f,   0.99531612f,   0.99533019f,   0.99534425f,
       0.99535832f,   0.99537238f,   0.99538645f,   0.99540051f,   0.99541458f,
       0.99542865f,   0.99544271f,   0.99545678f,   0.99547084f,   0.99548491f,
       0.99549897f,   0.99551304f,   0.99552711f,   0.99554117f,   0.99555524f,
       0.99556930f,   0.99558337f,   0.99559743f,   0.99561150f,   0.99562557f,
       0.99563963f,   0.99565370f,   0.99566776f,   0.99568183f,   0.99569589f,
       0.99570996f,   0.99572403f,   0.99573809f,   0.99575216f,   0.99576622f,
       0.99578029f,   0.99579435f,   0.99580842f,   0.99582249f,   0.99583655f,
       0.99585062f,   0.99586468f,   0.99587875f,   0.99589281f,   0.99590688f,
       0.99592095f,   0.99593501f,   0.99594908f,   0.99596314f,   0.99597721f,
       0.99599127f,   0.99600534f,   0.99601941f,   0.99603347f,   0.99604754f,
       0.99606160f,   0.99607567f,   0.99608973f,   0.99610380f,   0.99611787f,
       0.99613193f,   0.99614600f,   0.99616006f,   0.99617413f,   0.99618819f,
       0.99620226f,   0.99621633f,   0.99623039f,   0.99624446f,   0.99625852f,
       0.99627259f,   0.99628665f,   0.99630072f,   0.99631479f,   0.99632885f,
       0.99634292f,   0.99635698f,   0.99637105f,   0.99638511f,   0.99639918f,
       0.99641324f,   0.99642731f,   0.99644138f,   0.99645544f,   0.99646951f,
       0.99648357f,   0.99649764f,   0.99651170f,   0.99652577f,   0.99653984f,
       0.99655390f,   0.99656797f,   0.99658203f,   0.99659610f,   0.99661016f,
       0.99662423f,   0.99663830f,   0.99665236f,   0.99666643f,   0.99668049f,
       0.99669456f,   0.99670862f,   0.99672269f,   0.99673676f,   0.99675082f,
       0.99676489f,   0.99677895f,   0.99679302f,   0.99680708f,   0.99682115f,
       0.99683522f,   0.99684928f,   0.99686335f,   0.99687741f,   0.99689148f,
       0.99690554f,   0.99691961f,   0.99693368f,   0.99694774f,   0.99696181f,
       0.99697587f,   0.99698994f,   0.99700400f,   0.99701807f,   0.99703214f,
       0.99704620f,   0.99706027f,   0.99707433f,   0.99708840f,   0.99710246f,
       0.99711653f,   0.99713060f,   0.99714466f,   0.99715873f,   0.99717279f,
       0.99718686f,   0.99720092f,   0.99721499f,   0.99722906f,   0.99724312f,
       0.99725719f,   0.99727125f,   0.99728532f,   0.99729938f,   0.99731345f,
       0.99732752f,   0.99734158f,   0.99735565f,   0.99736971f,   0.99738378f,
       0.99739784f,   0.99741191f,   0.99742598f,   0.99744004f,   0.99745411f,
       0.99746817f,   0.99748224f,   0.99749630f,   0.99751037f,   0.99752444f,
       0.99753850f,   0.99755257f,   0.99756663f,   0.99758070f,   0.99759476f,
       0.99760883f,   0.99762290f,   0.99763696f,   0.99765103f,   0.99766509f,
       0.99767916f,   0.99769322f,   0.99770729f,   0.99772136f,   0.99773542f,
       0.99774949f,   0.99776355f,   0.99777762f,   0.99779168f,   0.99780575f,
       0.99781982f,   0.99783388f,   0.99784795f,   0.99786201f,   0.99787608f,
       0.99789014f,   0.99790421f,   0.99791828f,   0.99793234f,   0.99794641f,
       0.99796047f,   0.99797454f,   0.99798860f,   0.99800267f,   0.99801674f,
       0.99803080f,   0.99804487f,   0.99805893f,   0.99807300f,   0.99808706f,
       0.99810113f,   0.99811520f,   0.99812926f,   0.99814333f,   0.99815739f,
       0.99817146f,   0.99818552f,   0.99819959f,   0.99821366f,   0.99822772f,
       0.99824179f,   0.99825585f,   0.99826992f,   0.99828398f,   0.99829805f,
       0.99831212f,   0.99832618f,   0.99834025f,   0.99835431f,   0.99836838f,
       0.99838244f,   0.99839651f,   0.99841058f,   0.99842464f,   0.99843871f,
       0.99845277f,   0.99846684f,   0.99848090f,   0.99849497f,   0.99850904f,
       0.99852310f,   0.99853717f,   0.99855123f,   0.99856530f,   0.99857936f,
       0.99859343f,   0.99860750f,   0.99862156f,   0.99863563f,   0.99864969f,
       0.99866376f,   0.99867782f,   0.99869189f,   0.99870596f,   0.99872002f,
       0.99873409f,   0.99874815f,   0.99876222f,   0.99877628f,   0.99879035f,
       0.99880441f,   0.99881848f,   0.99883255f,   0.99884661f,   0.99886068f,
       0.99887474f,   0.99888881f,   0.99890287f,   0.99891694f,   0.99893101f,
       0.99894507f,   0.99895914f,   0.99897320f,   0.99898727f,   0.99900133f,
       0.99901540f,   0.99902947f,   0.99904353f,   0.99905760f,   0.99907166f,
       0.99908573f,   0.99909979f,   0.99911386f,   0.99912793f,   0.99914199f,
       0.99915606f,   0.99917012f,   0.99918419f,   0.99919825f,   0.99921232f,
       0.99922639f,   0.99924045f,   0.99925452f,   0.99926858f,   0.99928265f,
       0.99929671f,   0.99931078f,   0.99932485f,   0.99933891f,   0.99935298f,
       0.99936704f,   0.99938111f,   0.99939517f,   0.99940924f,   0.99942331f,
       0.99943737f,   0.99945144f,   0.99946550f,   0.99947957f,   0.99949363f,
       0.99950770f,   0.99952177f,   0.99953583f,   0.99954990f,   0.99956396f,
       0.99957803f,   0.99959209f,   0.99960616f,   0.99962023f,   0.99963429f,
       0.99964836f,   0.99966242f,   0.99967649f,   0.99969055f,   0.99970462f,
       0.99971869f,   0.99973275f,   0.99974682f,   0.99976088f,   0.99977495f,
       0.99978901f,   0.99980308f,   0.99981715f,   0.99983121f,   0.99984528f,
       0.99985934f,   0.99987341f,   0.99988747f,   0.99990154f,   0.99991561f,
       0.99992967f,   0.99994374f,   0.99995780f,   0.99997187f,   0.99998593f,
       1.00000000f };

                     int i = (int)ceil( x * (2000));
       return alpha1_3[i];
}

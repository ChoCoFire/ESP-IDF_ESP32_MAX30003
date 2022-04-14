#include <stdio.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include "driver/spi_master.h"

#include "esp_log.h"
#include "max30003.h"
#include "normalize.h"

void app_main(void)
{
    // //init SPI pin with your setup
    // max30003_initchip(PIN_SPI_MISO,PIN_SPI_MOSI,PIN_SPI_SCK,PIN_SPI_CS);
    // vTaskDelay(10/ portTICK_PERIOD_MS);

    // while(1)
    // {
    //     long db = max30003_read_ecg_data();
    //     ESP_LOGI("SWU_BME", "%ld", db);
    //     // printf("%ld, ", db);
    //     vTaskDelay(8/ portTICK_PERIOD_MS);
    // }

    ESP_LOGI("SWU_BME","Start");

    double temp[] = {-0.02020658,-0.01952792,-0.03325057,-0.02280943,-0.01713891,-0.01731929,-0.03738474,-0.04947155,-0.03855586,-0.0344236,-0.05647221,-0.05052745,-0.052598,-0.04451874,-0.06676342,-0.07975586,-0.07549316,-0.07238542,-0.08630058,-0.09959083,-0.0964334,-0.08314723,-0.07869979,-0.09756084,-0.08992108,-0.0849638,-0.08742371,-0.11341794,-0.12056649,-0.11016694,-0.11543112,-0.13404287,-0.12303958,-0.11675663,-0.10392572,-0.1251591,-0.13218583,-0.12035464,-0.10411682,-0.11960538,-0.11830811,-0.12010765,-0.11323493,-0.11588004,-0.11688356,-0.11535373,-0.09993175,-0.07280388,-0.05314332,-0.04778574,-0.03940291,-0.02974855,-0.04986165,-0.04402426,-0.02111136,-0.01143445,-0.01989136,-0.01809477,0.01063383,0.01389679,-0.00543234,-0.02830749,-0.03899034,-0.04501868,-0.0575387,-0.05415738,-0.04957488,-0.0448188,-0.06569888,-0.10766313,-0.10882261,-0.10156435,-0.13103032,-0.15658283,-0.15204188,-0.14040903,-0.15950781,-0.17419743,-0.17763115,-0.16136172,-0.16419331,-0.16815751,-0.17455453,-0.17361102,-0.16509462,-0.17224994,-0.17110622,-0.16285391,-0.15228124,-0.15886971,-0.18331823,-0.19145471,-0.27397652,-0.33637744,-0.29954538,-0.19088209,0.03261568,0.5012762,1.03831001,1.56053143,1.93025864,2.00401046,1.71900306,1.01226301,0.29602794,-0.14272766,-0.28736358,-0.27053461,-0.1346,-0.04515962,-0.08434208,-0.11966254,-0.12290753,-0.115921,-0.13750453,-0.16882443,-0.18014429,-0.17334699,-0.18176542,-0.19787432,-0.20074313,-0.18845426,-0.19619982,-0.21114689,-0.201084,-0.19373658,-0.20251175,-0.21264343,-0.20237118,-0.18347562,-0.18678099,-0.19929875,-0.19797847,-0.17583533,-0.17860617,-0.1856928,-0.18339895,-0.17113249,-0.17052084,-0.17246534,-0.16401046,-0.15518466,-0.12886827,-0.11880437,-0.13146063,-0.09832493,-0.06367485,-0.06978561,-0.06911007,-0.04060581,-0.00841477,0.0036117,0.01083235,0.03488598,0.0724483,0.09260204,0.11979201,0.14195668,0.18797103,0.1914611,0.20064849,0.22156365,0.25717263,0.28053464,0.2831158,0.29496346,0.31851926,0.33843904,0.33479628,0.34165878,0.34761387,0.34586977,0.33566047,0.31427849,0.30294516,0.28142694,0.24854078,0.20291195,0.17477624,0.15920532,0.1353916,0.09560134,0.0728086,0.05757519,0.04003838,0.00509095,-0.02316073,-0.00101189,-0.00129285,-0.01655339,-0.04005851,-0.03635674,-0.02303467,-0.05131423,-0.05233877,-0.04143843,-0.03032201,-0.03146186,-0.04011011,-0.03618732,-0.02800661,-0.02723243,-0.03030143,-0.03323169,-0.01906708,-0.01053063,-0.02740485,-0.03692075,-0.01908647,-0.00905707,-0.02145452,-0.01640049,-0.00357339,0.00841953,-0.00035387,-0.00827977,0.00576449,0.00593521,-0.00556823,-0.00354883,-0.00863918,-0.0025767,-0.00428614,-0.01713193,-0.01538911,-0.0106461,-0.01485228,-0.02918565,-0.03275118,-0.02852895,-0.00900644,-0.02761659,-0.04250854,-0.02959648,-0.02552817,-0.04577497,-0.05819008,-0.05902489,-0.0525494,-0.06488,-0.07427523,-0.08074806,-0.0586737,-0.07248849,-0.09728022,-0.09695729,-0.08485241,-0.09069497,-0.11031778,-0.10117781,-0.10718204,-0.10526312,-0.10575619,-0.10569246,-0.08564862,-0.08598184,-0.10197903,-0.11460749,-0.08749294,-0.10228301,-0.10502685,-0.11116173,-0.10473892,-0.10014749,-0.10846032,-0.11099372,-0.09450185,-0.07932526,-0.06555314,-0.05060204,-0.04441657,-0.01771304,-0.01755489,-0.02997004,-0.02033715,0.01132368,0.01110347,-0.01782661,-0.01497401,0.0152995,0.01195729,-0.00322801,-0.02558848,-0.02906784,-0.04125169,-0.04918969,-0.04211599,-0.02982738,-0.02175129,-0.04854768,-0.08638963,-0.08585842,-0.08343984,-0.09877367,-0.13008183,-0.13205053,-0.11569286,-0.13460372,-0.14619297,-0.15066209,-0.14476296,-0.14021247,-0.15918531,-0.16631365,-0.14922633,-0.14467308,-0.15984134,-0.15460681,-0.14535327,-0.12838021,-0.14464894,-0.16321918,-0.15424957,-0.16324489,-0.22384929,-0.28607328,-0.26132828,-0.17544739,-0.04907445,0.20550966,0.65872607,1.11495273,1.51315614,1.80624246,1.94357317,1.79360993,1.21784526,0.49333006,-0.0321834,-0.25014238,-0.25977337,-0.17811585,-0.06872846,-0.03956486,-0.06723143,-0.1101253,-0.12412716,-0.10680653,-0.10672165,-0.14652999,-0.17362077,-0.16437827,-0.16829644,-0.17203642,-0.18033884,-0.18199783,-0.18371815,-0.16906023,-0.1945744,-0.17415156,-0.17210666,-0.16116501,-0.18647142,-0.17817688,-0.16832399,-0.1614967,-0.1702166,-0.18153312,-0.17384508,-0.16511366,-0.15509819,-0.1649805,-0.15937891,-0.13646945,-0.14046654,-0.13599028,-0.11443285,-0.09796952,-0.10108266,-0.10092536,-0.08480773,-0.06055012,-0.06027548,-0.05301646,-0.03750937,0.00865136,0.02177732,0.02471801,0.05879576,0.09923653,0.11441108,0.12393402,0.14499178,0.1787563,0.21777914,0.22596386,0.22964914,0.25276377,0.2851588,0.29653086,0.30722584,0.31633877,0.34147127,0.34950063,0.33964474,0.35063238,0.36904079,0.36126147,0.33080091,0.32006111,0.3131598,0.28890762,0.24906454,0.21755857,0.20511246,0.17904088,0.1355504,0.09723325,0.08256436,0.0695683,0.0294419,0.0149585,0.01048759,-0.00171062,-0.0164658,-0.03253901,-0.03143369,-0.03007466,-0.03663726,-0.05496676,-0.04560882,-0.03521796,-0.03772988,-0.05914363,-0.06112158,-0.05050738,-0.04694917,-0.05169674,-0.03725198,-0.0313741,-0.02169785,-0.03154021,-0.0446739,-0.01866269,-0.01234918,-0.0211173,-0.03242344,-0.00982429,0.00173542,-0.00333333,-0.0064053,0.01123292,0.01372497,-0.00212411,-0.01266956,-0.00187422,0.00788214,0.01009704,-0.00472707,0.00317467,0.02539794,0.01046123,-0.00980739,0.00083872,0.01141362,0.00812842,-0.00640801,-0.02121334,-0.00723836,-0.00594274,-0.01777813,-0.03659051,-0.02448335,-0.02062779,-0.03623353,-0.05102795,-0.04484921,-0.03799834,-0.04893434,-0.07332803,-0.06760614,-0.05150738,-0.05862289,-0.07590455,-0.0732215,-0.06034368,-0.06645973,-0.07514404,-0.08778523,-0.08267239,-0.07766532,-0.08644425,-0.08144316,-0.08292457,-0.0779085,-0.09663917,-0.0984478,-0.10055153,-0.08717835,-0.10055102,-0.09847543,-0.0896718,-0.07904137,-0.08322702,-0.07989305,-0.06649566,-0.03579664,-0.02721134,-0.02454722,-0.02260674,-0.00870532,-0.00082383,0.01044855,0.01264708,0.03038371,0.02920337,0.02718626,0.02875943,0.02717168,0.02033135,-0.00781936,-0.02896757,-0.02097083,-0.00926348,-0.00252304,-0.01577661,-0.02975161,-0.04930457,-0.07141004,-0.09784983,-0.11102754,-0.10742934,-0.11913988,-0.1394251,-0.1550481,-0.14885784,-0.14219813,-0.16271369,-0.15240526,-0.14304066,-0.14139517,-0.15131006,-0.15824421,-0.14580503,-0.13813082,-0.14975501,-0.15344399,-0.16940896,-0.20953032,-0.32255362,-0.34337299,-0.26076935,-0.09600716,0.24946731,0.77193613,1.33802617,1.84970221,2.14649837,2.1138749,1.60347541,0.77220534,0.05784323,-0.25880314,-0.2737086,-0.16325623,-0.01782397,-0.02774162,-0.07573276,-0.09964967,-0.09432141,-0.11965683,-0.14720023,-0.16144381,-0.15400253,-0.16867007,-0.19038923,-0.19304858,-0.18034708,-0.19422838,-0.20124308,-0.19616538,-0.18085302,-0.18138171,-0.19840338,-0.19408826,-0.18012271,-0.17871598,-0.18423487,-0.18660831,-0.16686917,-0.15726418,-0.17212146,-0.17547491,-0.14421411,-0.13364586,-0.14620547,-0.13983866,-0.11301147,-0.10161776,-0.09973075,-0.09182939,-0.06034199,-0.03672696,-0.01957224,-0.00797995,0.01665328,0.05521215,0.07910385,0.09497222,0.11481608,0.16254651,0.19152501,0.19890114,0.22555218,0.27400863,0.29676525,0.3017984,0.32038096,0.3529687,0.3700551,0.37051108,0.37377992,0.39913834,0.38996884,0.37270861,0.36302503,0.35727268,0.33718668,0.30410708,0.26728829,0.24068461,0.2193941,0.16804949,0.11661039,0.0946225,0.07102856,0.04567915,0.01171731,0.00023331,-0.00298572,-0.01282412,-0.02948558,-0.0436844,-0.04537984,-0.04273261,-0.03701994,-0.0566522,-0.04330952,-0.0363281,-0.05488036,-0.05910964,-0.04744827,-0.03413998,-0.03503543,-0.04922945,-0.03383479,-0.02048423,-0.01970134,-0.02385732,-0.01866672,-0.00793584,-0.00336999,-0.01484027,-0.02152718,0.00476429,0.00806665,-0.01310474,-0.00757067,-0.00276258,0.00765412,-0.00309915,-0.01165098,-0.00939687,0.00342965,-0.0086636,-0.01885701,-0.01542001,-0.01052912,-0.01483774,-0.02563235,-0.02657755,-0.01384886,-0.02614154,-0.03633475,-0.03613625,-0.03436369,-0.03737062,-0.05615501,-0.05629291,-0.0278752,-0.0514496,-0.06037337,-0.08164668,-0.0808033,-0.07654639,-0.08269708,-0.10334756,-0.09212215,-0.08970707,-0.10104886,-0.10182851,-0.10005186,-0.09537812,-0.11065359,-0.10911451,-0.10800368,-0.09170791,-0.09494012,-0.10963651,-0.11265269,-0.09872014,-0.09548039,-0.11808855,-0.11859091,-0.09473811,-0.10379286,-0.1025719,-0.09006225,-0.0708302,-0.02906475,-0.03197939,-0.04864637,-0.0198329,-0.02908065,-0.01112948,-0.01280547,0.00411135,0.0122778,0.00741973,-0.00082391,0.00068867,-0.01173044,-0.03342328,-0.06027214,-0.03706641,-0.02040667,-0.03984452,-0.07590023,-0.08985414,-0.10066594,-0.10403237,-0.13622463,-0.15049606,-0.13545461,-0.15081031,-0.15933803,-0.17820611,-0.16088873,-0.15980776,-0.15954664,-0.17552459,-0.16934521,-0.14392594,-0.14932973,-0.17161338,-0.1635818,-0.1720304,-0.21724185,-0.33709216,-0.33860371};
    double *result;
    result = getNormalize(temp,750,-1,2);
    for(int i=0;i<750;i++)  
    {  
        ESP_LOGI("SWU_BME", "%f", result[i]);
    }  

    ESP_LOGI("SWU_BME","End");

}

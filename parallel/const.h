namespace {
  uint8_t MANHATTAN_DIST[16][16] = {
    {0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0 ,0},
    {1 ,0 ,1 ,2 ,2 ,1 ,2 ,3 ,3 ,2 ,3 ,4 ,4 ,3 ,4 ,5},
    {2 ,1 ,0 ,1 ,3 ,2 ,1 ,2 ,4 ,3 ,2 ,3 ,5 ,4 ,3 ,4},
    {3 ,2 ,1 ,0 ,4 ,3 ,2 ,1 ,5 ,4 ,3 ,2 ,6 ,5 ,4 ,3},
    {1 ,2 ,3 ,4 ,0 ,1 ,2 ,3 ,1 ,2 ,3 ,4 ,2 ,3 ,4 ,5},
    {2 ,1 ,2 ,3 ,1 ,0 ,1 ,2 ,2 ,1 ,2 ,3 ,3 ,2 ,3 ,4},
    {3 ,2 ,1 ,2 ,2 ,1 ,0 ,1 ,3 ,2 ,1 ,2 ,4 ,3 ,2 ,3},
    {4 ,3 ,2 ,1 ,3 ,2 ,1 ,0 ,4 ,3 ,2 ,1 ,5 ,4 ,3 ,2},
    {2 ,3 ,4 ,5 ,1 ,2 ,3 ,4 ,0 ,1 ,2 ,3 ,1 ,2 ,3 ,4},
    {3 ,2 ,3 ,4 ,2 ,1 ,2 ,3 ,1 ,0 ,1 ,2 ,2 ,1 ,2 ,3},
    {4 ,3 ,2 ,3 ,3 ,2 ,1 ,2 ,2 ,1 ,0 ,1 ,3 ,2 ,1 ,2},
    {5 ,4 ,3 ,2 ,4 ,3 ,2 ,1 ,3 ,2 ,1 ,0 ,4 ,3 ,2 ,1},
    {3 ,4 ,5 ,6 ,2 ,3 ,4 ,5 ,1 ,2 ,3 ,4 ,0 ,1 ,2 ,3},
    {4 ,3 ,4 ,5 ,3 ,2 ,3 ,4 ,2 ,1 ,2 ,3 ,1 ,0 ,1 ,2},
    {5 ,4 ,3 ,4 ,4 ,3 ,2 ,3 ,3 ,2 ,1 ,2 ,2 ,1 ,0 ,1},
    {6 ,5 ,4 ,3 ,5 ,4 ,3 ,2 ,4 ,3 ,2 ,1 ,3 ,2 ,1 ,0}
  };

  uint64_t ZOBRIST_HASH[16][16] =
  {
    {
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0,
      0
    },
    {
      2194202508838793468,
      1304815685499682304,
      8195027001007772223,
      16594729317912875332,
      2535129823708353427,
      11994794626899286097,
      8130720575886066984,
      4544480479134768901,
      16099596206416860419,
      17196720873683834474,
      2541662032650256158,
      12673749047831905457,
      15505130878002809546,
      16358469545245535822,
      4883927881197267352,
      10787718384162316515
    },
    {
      18259529419726553069,
      16688097420397800439,
      13296545236197994130,
      8214894832486036807,
      11621621902359057920,
      8959954635984636943,
      13219501550655528165,
      12656114935164908746,
      5413697662907522581,
      9974906196240319585,
      9376043882154912192,
      2485273481530467630,
      15199151494976760026,
      15128085036458813189,
      13041188816204021471,
      4639340534197781412
    },
    {
      5697271810632639742,
      5280811427758496823,
      14301695974753064819,
      6283565437403441705,
      12446213555912048017,
      11181930034927894939,
      9235805256263934290,
      950233735318419124,
      13510054758217700272,
      12538358788509159874,
      9976782673811389873,
      7307276218903014913,
      13798257699646115089,
      4703409893484117299,
      7487824078410329237,
      12002058526258526613
    },
    {
      13707309280933811954,
      274002628743287638,
      144232516662291239,
      10380958806145833689,
      6905626775066271615,
      563877272144051713,
      14860783454616687203,
      1395637348978558575,
      1425898901124408863,
      3897369888438266430,
      2192669894033101993,
      17223323683266066950,
      1459959039172681313,
      14060489985733499321,
      8567566547471502984,
      17955936540222299577
    },
    {
      12034917914076612532,
      3405006676343317440,
      8189966501209126740,
      7834594665562053455,
      2595738096308655511,
      14195232382745738819,
      10610550275175250106,
      10922107586719449623,
      8660772747614426554,
      6135737716524387163,
      4871096055416745310,
      6422039615331793276,
      6020047640073822287,
      18118473765604457829,
      2507131535124914729,
      5405082061172034809
    },
    {
      8979223607852622641,
      245346656901931787,
      1131329433654258005,
      8413426556629399487,
      162341361374505132,
      4762096921118386917,
      16153738498364108562,
      16728071384168367863,
      9964622036805959963,
      4918151452257696440,
      1487548497390391629,
      12489309044830047319,
      5319698288167780098,
      10493036453128381242,
      14642915396426242422,
      5426702336687171758
    },
    {
      9735994632066767770,
      14199747735489584472,
      12498051323519532536,
      16066164852023570914,
      9750920711053619279,
      16256114378671150980,
      10832832769138682545,
      18129530552740333339,
      3637841263797053226,
      18171976487797070161,
      7444700063236010971,
      3981175951469890327,
      2894817172397843045,
      13568393465727855475,
      14875053916684791602,
      8901810777923218897
    },
    {
      11186182115847206330,
      1768501342313794185,
      11417829632458102413,
      14451715154411899546,
      6746597683640577470,
      4243505373144506068,
      11779537924977521179,
      6955650586098693204,
      11018399975596222822,
      13179698211460927280,
      4667183512602389434,
      5561266045521908295,
      267817693054365124,
      15293055419458314635,
      9348906336763040893,
      6323783340539902056
    },
    {
      9387232123642696668,
      12087945649363936045,
      3076435293967780577,
      8050828244024007481,
      74489093364279449,
      6374996770076481584,
      4741166062853375756,
      1950845401199566104,
      298308829264011113,
      10231646234762090257,
      10319803879786839742,
      12775310704414886975,
      4509189050416098011,
      2876879275120228836,
      13239375444982990029,
      17511414640541365097
    },
    {
      3074632669701183689,
      2850147097714574052,
      12601929316310491579,
      10412106305936611503,
      15693415077262063705,
      12227908466775894575,
      1846341898953735360,
      10448275682905902873,
      8383287849381580003,
      7275276033798315935,
      12356154146571381846,
      12699366980098277189,
      1767547403567077574,
      3603411806788422322,
      2123909071806829894,
      475463063813823429
    },
    {
      1498229321209795367,
      17711573948689519316,
      6229247981589140489,
      316992282963186493,
      228171108492231815,
      5319260092429072439,
      6010114929796711039,
      12998437691641913088,
      676418378185917136,
      3670527732717380696,
      7052077804920506112,
      12670855732255359643,
      9080126389216862806,
      9590389592391235400,
      2856193239399659183,
      7012935680206749603
    },
    {
      975923871642365140,
      11941749928588106976,
      14551822310026374488,
      11998525630070511618,
      1529756053998802670,
      9368955713734178967,
      17942213334389477369,
      10208068382234302801,
      10679372689631147822,
      9149552229726429870,
      8394597782814296938,
      14347281444176736123,
      1178457039433556986,
      15153879420389688895,
      499110418482931364,
      4780867979637086651
    },
    {
      3528090656981480597,
      1130782108367904473,
      6101013077862210456,
      15323189235249626621,
      6672686509442211444,
      13933198531242386212,
      9753941888938382164,
      3666314609311262603,
      14190670615170956644,
      15283458298093437243,
      12600213248800515562,
      18339577713722673657,
      743268359809056447,
      16211920941087215468,
      8689431496804105338,
      6440883418498343935
    },
    {
      10020694762133937399,
      62224053187318000,
      6747074654797375811,
      7440166812897185944,
      14534433304092934582,
      5925869824945773715,
      14250085683905250534,
      8079318780125454806,
      1925690159454480310,
      3196167363814687993,
      16083078044777961296,
      2095468851133106424,
      17075133709754661150,
      2986282515083576453,
      5385787810122652044,
      17105055154221593904
    },
    {
      2296677289588855565,
      1416881662084958799,
      3359015099664327052,
      6297997967288546822,
      7801007225819645657,
      936980177206818308,
      10627323761321012888,
      15024994416949970145,
      2629596082433010673,
      14544674314239238270,
      7693632611324261612,
      656463885355286032,
      17701323612389072903,
      10574489976075043049,
      5167629239803895803,
      6545642047219219424
    }
  };
}
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <algorithm>

const unsigned int PREFIXES_COUNT = 395;
const char *PREFIXES[ PREFIXES_COUNT ] = {
    "DJ",
    "DL",
    "DB",
    "DW",
    "DBL",
    "DDZ",
    "DGL",
    "DGR",
    "DJA",
    "DJE",
    "DKA",
    "DKL",
    "DLE",
    "DLB",
    "DLU",
    "DLW",
    "DMI",
    "DOL",
    "DOA",
    "DPL",
    "DST",
    "DSR",
    "DSW",
    "DTR",
    "DBA",
    "DWL",
    "DWR",
    "DZA",
    "DZG",
    "DZL",
    "CB",
    "CG",
    "CT",
    "CW",
    "CAL",
    "CBR",
    "CBY",
    "CCH",
    "CGD",
    "CGR",
    "CIN",
    "CLI",
    "CMG",
    "CNA",
    "CRA",
    "CRY",
    "CSE",
    "CSW",
    "CTR",
    "CTU",
    "CWA",
    "CWL",
    "CZN",
    "LB",
    "LC",
    "LU",
    "LZ",
    "LBI",
    "LBL",
    "LCH",
    "LHR",
    "LJA",
    "LKS",
    "LKR",
    "LLB",
    "LUB",
    "LLE",
    "LLU",
    "LOP",
    "LPA",
    "LPU",
    "LRA",
    "LRY",
    "LSW",
    "LTM",
    "LWL",
    "LZA",
    "FG",
    "FZ",
    "FGW",
    "FKR",
    "FMI",
    "FNW",
    "FSL",
    "FSD",
    "FSU",
    "FSW",
    "FWS",
    "FZI",
    "FZG",
    "FZA",
    "EL",
    "EP",
    "ES",
    "EBE",
    "EBR",
    "EKU",
    "ELA",
    "ELE",
    "ELC",
    "ELW",
    "EOP",
    "EPA",
    "EPJ",
    "EPI",
    "EPD",
    "ERA",
    "ERW",
    "ESI",
    "ESK",
    "ETM",
    "EWI",
    "EWE",
    "EZD",
    "EZG",
    "KR",
    "KK",
    "KN",
    "KT",
    "KBC",
    "KBR",
    "KCH",
    "KDA",
    "KGR",
    "KRA",
    "KLI",
    "KMI",
    "KMY",
    "KNS",
    "KNT",
    "KOL",
    "KOS",
    "KPR",
    "KSU",
    "KTA",
    "KTT",
    "KWA",
    "KWI",
    "WB",
    "WA",
    "WD",
    "WE",
    "WU",
    "WF",
    "WH",
    "WI",
    "WJ",
    "WK",
    "WN",
    "WT",
    "WY",
    "WX",
    "WO",
    "WP",
    "WR",
    "WS",
    "WBR",
    "WCI",
    "WG",
    "WGS",
    "WGM",
    "WGR",
    "WKZ",
    "WL",
    "WLI",
    "WLS",
    "WMA",
    "WM",
    "WML",
    "WND",
    "WOS",
    "WOR",
    "WOT",
    "WPI",
    "WPL",
    "WPN",
    "WPR",
    "WPZ",
    "WPY",
    "WPU",
    "WRA",
    "WSI",
    "WSE",
    "WSC",
    "WSK",
    "WSZ",
    "WZ",
    "WWE",
    "WWL",
    "WWY",
    "WZW",
    "WZU",
    "WZY",
    "OP",
    "OB",
    "OGL",
    "OK",
    "OKL",
    "OKR",
    "ONA",
    "ONY",
    "OOL",
    "OPO",
    "OPR",
    "OST",
    "RK",
    "RP",
    "RZ",
    "RT",
    "RBI",
    "RBR",
    "RDE",
    "RJA",
    "RJS",
    "RKL",
    "RKR",
    "RLS",
    "RLE",
    "RLU",
    "RLA",
    "RMI",
    "RNI",
    "RPR",
    "RPZ",
    "RRS",
    "RZE",
    "RSA",
    "RST",
    "RSR",
    "RTA",
    "BI",
    "BL",
    "BS",
    "BAU",
    "BIA",
    "BBI",
    "BGR",
    "BHA",
    "BKL",
    "BLM",
    "BMN",
    "BSE",
    "BSI",
    "BSK",
    "BSU",
    "BWM",
    "BZA",
    "GD",
    "GA",
    "GS",
    "GSP",
    "GBY",
    "GCH",
    "GCZ",
    "GDA",
    "GKA",
    "GKS",
    "GKW",
    "GLE",
    "GMB",
    "GND",
    "GPU",
    "GSL",
    "GST",
    "GSZ",
    "GTC",
    "GWE",
    "SB",
    "SY",
    "SH",
    "SC",
    "SD",
    "SG",
    "SJZ",
    "SJ",
    "SK",
    "SM",
    "SPI",
    "SL",
    "SR",
    "SI",
    "SO",
    "SW",
    "ST",
    "SZ",
    "SZO",
    "SBE",
    "SBI",
    "SBL",
    "SCI",
    "SCZ",
    "SGL",
    "SKL",
    "SLU",
    "SMI",
    "SMY",
    "SPS",
    "SRC",
    "SRB",
    "STA",
    "SWD",
    "SZA",
    "SZY",
    "TK",
    "TBU",
    "TJE",
    "TKA",
    "TKI",
    "TKN",
    "TOP",
    "TOS",
    "TPI",
    "TSA",
    "TSK",
    "TST",
    "TSZ",
    "TLW",
    "NE",
    "NO",
    "NBA",
    "NBR",
    "NDZ",
    "NEB",
    "NEL",
    "NGI",
    "NGO",
    "NIL",
    "NKE",
    "NLI",
    "NMR",
    "NNI",
    "NNM",
    "NOE",
    "NOL",
    "NOS",
    "NPI",
    "NSZ",
    "NWE",
    "PK",
    "PN",
    "PL",
    "PO",
    "PY",
    "PCH",
    "PCT",
    "PGN",
    "PGS",
    "PGO",
    "PJA",
    "PKA",
    "PKE",
    "PKL",
    "PKN",
    "PKS",
    "PKR",
    "PLE",
    "PMI",
    "PNT",
    "POB",
    "POS",
    "POT",
    "PP",
    "PPL",
    "PZ",
    "PRA",
    "PSL",
    "PSZ",
    "PSE",
    "PSR",
    "PTU",
    "PWA",
    "PWL",
    "PWR",
    "PZL",
    "ZK",
    "ZS",
    "ZSW",
    "ZBI",
    "ZCH",
    "ZDR",
    "ZGL",
    "ZGY",
    "ZGR",
    "ZKA",
    "ZKL",
    "ZKO",
    "ZLO",
    "ZMY",
    "ZPL",
    "ZPY",
    "ZSL",
    "ZST",
    "ZSZ",
    "ZSD",
    "ZWA"
};

inline void PrintToFile( std::ofstream &file, const std::string &elem ) {
    file << std::setw( 9 ) << elem;
}

int main( int argc, char **argv ) {
    if ( argc < 3 ) {
        perror( "Podaj nazwie pliku docelowego i ilosc rekordow!\n" );
        return EXIT_FAILURE;
    }

    char const * const PARAM_FILE_PATH = argv[ 1 ];
    const unsigned int PARAM_RECORDS_COUNT = std::stoi( std::string( argv[ 2 ] ) );

    if ( PARAM_RECORDS_COUNT > 90000 * 200 ) {
        perror( "Za duzo rekordow!\n" );
        return EXIT_FAILURE;
    }

    std::srand( unsigned( std::time( 0 ) ) );
    std::vector<std::string> elements;
    for ( int i = 10000; i < PARAM_RECORDS_COUNT + 10000; i++ ) {
        char elem[ 10 ];
        unsigned int rand = ( unsigned int )( ( PREFIXES_COUNT - 1 ) * ( std::rand() / ( RAND_MAX + 1.0 ) ) );
        //for ( unsigned int j = rand, k = 0; k < 200; j = ( j + 1 ) % PREFIXES_COUNT, k++ ) {
            sprintf_s( elem, 10, "%s%d\0", PREFIXES[ rand /* j */ ], i );
            elements.push_back( std::string( elem ) );
        //}
    }
    std::random_shuffle( elements.begin(), elements.end() );

    std::ofstream outFile( PARAM_FILE_PATH );
    if ( !outFile.good() ) {
        perror( "Plik docelowy nie mogl byc otwarty!\n" );
        return EXIT_FAILURE;
    }

    for ( auto& elem : elements ) {
        PrintToFile( outFile, elem );
    }

    outFile.close();
    return EXIT_SUCCESS;
}
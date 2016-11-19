#include "gtest/gtest.h"
#include "bigfilesort.h"

#include <sstream>
#include <fstream>
#include <memory>

using namespace bfs;

struct DistributeTest : public ::testing::Test {
    DistributeTest() : ssOutTape( new std::stringstream() ), ssOutTape2( new std::stringstream() ), ssInTape( new std::stringstream() ),
        outTape( std::dynamic_pointer_cast< std::iostream >( ssOutTape ), 3, 9 ),
        outTape2( std::dynamic_pointer_cast< std::iostream >( ssOutTape2 ), 3, 9 ),
        inTape( std::dynamic_pointer_cast< std::iostream >( ssInTape ), 3, 9 ) {};

    void SetUp() override {
        // First series
        AddToInTapeStream( "ABC12345" );
        AddToInTapeStream( "ABC67890" );
        AddToInTapeStream( "DEF12345" );
        AddToInTapeStream( "DEF67890" );
        AddToInTapeStream( "GHI12345" );
        // Second series
        AddToInTapeStream( "AB12345" );
        AddToInTapeStream( "GHI67890" );
        AddToInTapeStream( "TUW12345" );
        AddToInTapeStream( "XYZ67890" );
        // Third series
        AddToInTapeStream( "JKL67890" );
        AddToInTapeStream( "MNO12345" );
        AddToInTapeStream( "PRS67890" );
        // Fourth series
        AddToInTapeStream( "AB09876" );
        AddToInTapeStream( "CD67890" );
        AddToInTapeStream( "EF12345" );
        AddToInTapeStream( "GH67890" );
        // Fifth series
        AddToInTapeStream( "GH12345" );
    }

    std::shared_ptr<std::stringstream> ssOutTape, ssOutTape2, ssInTape;
    OutputTape outTape, outTape2;
    InputTape inTape;

protected:
    inline void AddToInTapeStream( std::string elem ) {
        *ssInTape << std::setw( inTape.RECORD_SIZE ) << elem;
    }
};

struct MergeTest : public ::testing::Test {
    MergeTest() : ssInTape( new std::stringstream() ), ssInTape2( new std::stringstream() ), ssOutTape( new std::stringstream() ),
        inTape( std::dynamic_pointer_cast< std::iostream >( ssInTape ), 3, 9 ),
        inTape2( std::dynamic_pointer_cast< std::iostream >( ssInTape2 ), 3, 9 ),
        outTape( std::dynamic_pointer_cast< std::iostream >( ssOutTape ), 3, 9 ) {};

    void SetUp() override {
        // First series
        AddToInTapeStream( "ABC12345", ssInTape );
        AddToInTapeStream( "ABC67890", ssInTape );
        AddToInTapeStream( "DEF67890", ssInTape );
        // Second series
        AddToInTapeStream( "AB09876", ssInTape );
        AddToInTapeStream( "CD67890", ssInTape );
        AddToInTapeStream( "EF12345", ssInTape );
        AddToInTapeStream( "GH67890", ssInTape );

        // First series
        AddToInTapeStream( "AB12345", ssInTape2 );
        AddToInTapeStream( "DEF12345", ssInTape2 );
        AddToInTapeStream( "GHI12345", ssInTape2 );
        AddToInTapeStream( "GHI67890", ssInTape2 );
        // Second series
        AddToInTapeStream( "GH12345", ssInTape2 );
    }

    std::shared_ptr<std::stringstream> ssInTape, ssInTape2, ssOutTape;
    InputTape inTape, inTape2;
    OutputTape outTape;

protected:
    inline void AddToInTapeStream( std::string elem, std::shared_ptr<std::stringstream> stream ) {
        *stream << std::setw( inTape.RECORD_SIZE ) << elem;
    }
};

struct SortTest : public ::testing::Test {
    bool PrepareSmallSet() {
        std::fstream file( FILE_NAME, std::ios::out | std::ios::trunc );
        if ( file.bad() ) {
            return false;
        }

        // First series
        AddToFileStream( "ABC12345", file );
        AddToFileStream( "DEF67890", file );
        // Second series
        AddToFileStream( "AB12345", file );
        AddToFileStream( "XYZ67890", file );
        // Third series
        AddToFileStream( "JKL67890", file );
        AddToFileStream( "MNO12345", file );
        AddToFileStream( "PRS67890", file );
        // Fourth series
        AddToFileStream( "AB09876", file );

        file.close();
        sorted = "  AB09876  AB12345 ABC12345 DEF67890 JKL67890 MNO12345 PRS67890 XYZ67890";

        return true;
    }

    bool PrepareBigSet() {
        std::fstream file( FILE_NAME, std::ios::out | std::ios::trunc );
        if ( file.bad() ) {
            return false;
        }

        // First series
        AddToFileStream( "ABC12345", file );
        AddToFileStream( "ABC67890", file );
        AddToFileStream( "DEF12345", file );
        AddToFileStream( "DEF67890", file );
        AddToFileStream( "GHI12345", file );
        // Second series
        AddToFileStream( "AB12345", file );
        AddToFileStream( "GHI67890", file );
        AddToFileStream( "TUW12345", file );
        AddToFileStream( "XYZ67890", file );
        // Third series
        AddToFileStream( "JKL67890", file );
        AddToFileStream( "MNO12345", file );
        AddToFileStream( "PRS67890", file );
        // Fourth series
        AddToFileStream( "AB09876", file );
        AddToFileStream( "CD67890", file );
        AddToFileStream( "EF12345", file );
        AddToFileStream( "GH67890", file );
        // Fifth series
        AddToFileStream( "GH12345", file );

        file.close();
        sorted = "  AB09876  AB12345 ABC12345 ABC67890  CD67890 DEF12345 DEF67890  EF12345  GH12345  GH67890 GHI12345 GHI67890 JKL67890 MNO12345 PRS67890 TUW12345 XYZ67890";

        return true;
    }

    bool PrepareSortedSet() {
        std::fstream file( FILE_NAME, std::ios::out | std::ios::trunc );
        if ( file.bad() ) {
            return false;
        }

        AddToFileStream( "ABC12345", file );
        AddToFileStream( "ABC67890", file );
        AddToFileStream( "DEF12345", file );
        AddToFileStream( "DEF67890", file );
        AddToFileStream( "GHI12345", file );
        AddToFileStream( "JKL67890", file );
        AddToFileStream( "MNO12345", file );
        AddToFileStream( "PRS67890", file );

        file.close();
        sorted = " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 JKL67890 MNO12345 PRS67890";

        return true;
    }

    std::string sorted;
    const std::string FILE_NAME = "test.txt";
    const uint64_t MAX_RECORDS_ON_PAGE = 3;
    const uint64_t RECORD_SIZE = 9;

protected:
    inline void AddToFileStream( std::string elem, std::fstream &file ) {
        file << std::setw( 9 ) << elem;
    }
};

TEST_F( DistributeTest, GIVENemptyInputTapeWHENdistributeToTwoOutputTapesTHENfunctionReturnsFalse ) {
    ssInTape->str( "" );
    ssInTape->clear();

    auto ret = distribute_series( inTape, outTape, outTape2 );

    EXPECT_FALSE( ret );
}

TEST_F( DistributeTest, GIVENnotZeroSeriesCountForOutputTapesWHENdistributeTHENfunctionReturnsFalse ) {
    outTape.pullSeries( inTape );
    outTape2.pullSeries( inTape );

    auto ret = distribute_series( inTape, outTape, outTape2 );

    EXPECT_FALSE( ret );
}

TEST_F( DistributeTest, GIVENfullInputTapeFibonacciUndistributableWHENdistributeTHENproperDistributionAndSeriesCounts ) {
    auto ret = distribute_series( inTape, outTape, outTape2 );
    outTape.flush();
    outTape2.flush();

    EXPECT_TRUE( ret );
    EXPECT_EQ( inTape.pop(), "" );

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 JKL67890 MNO12345 PRS67890  AB09876  CD67890  EF12345  GH67890" );
    EXPECT_EQ( outTape.getSeriesCount(), 2 );

    EXPECT_EQ( ssOutTape2->str(), "  AB12345 GHI67890 TUW12345 XYZ67890  GH12345" );
    EXPECT_EQ( outTape2.getSeriesCount(), 2 );
    EXPECT_EQ( outTape2.getEmptySeriesCount(), 1 );
}

TEST_F( DistributeTest, GIVENfullInputTapeFibonacciDistributableWHENdistributeTHENproperDistributionAndSeriesCounts ) {
    // Sixth series
    AddToInTapeStream( "EF67890" );

    auto ret = distribute_series( inTape, outTape, outTape2 );
    outTape.flush();
    outTape2.flush();

    EXPECT_TRUE( ret );
    EXPECT_EQ( inTape.pop(), "" );

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 JKL67890 MNO12345 PRS67890  AB09876  CD67890  EF12345  GH67890" );
    EXPECT_EQ( outTape.getSeriesCount(), 2 );

    EXPECT_EQ( ssOutTape2->str(), "  AB12345 GHI67890 TUW12345 XYZ67890  GH12345  EF67890" );
    EXPECT_EQ( outTape2.getSeriesCount(), 3 );
    EXPECT_EQ( outTape2.getEmptySeriesCount(), 0 );
}

TEST_F( MergeTest, GIVENtwoEmptyInputTapesWHENmergeIntoOutputTapeTHENfunctionReturnsFalse ) {
    ssInTape->str( "" );
    ssInTape->clear();
    ssInTape2->str( "" );
    ssInTape2->clear();

    auto ret = merge_series( outTape, inTape, inTape2 );

    EXPECT_FALSE( ret );
}

TEST_F( MergeTest, GIVENtwoFullInputTapesWHENmergeIntoOutputTapeTHENproperElementsInOuputTape ) {
    auto ret = merge_series( outTape, inTape, inTape2 );
    outTape.flush();

    EXPECT_TRUE( ret );
    EXPECT_EQ( ssOutTape->str(), "  AB12345 ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 GHI67890" );
}

TEST_F( MergeTest, GIVENtwoFullInputTapesWHENmergeTwiceIntoOutputTapeTHENproperElementsInOuputTape ) {
    auto ret = merge_series( outTape, inTape, inTape2 );
    outTape.flush();

    EXPECT_TRUE( ret );
    EXPECT_EQ( ssOutTape->str(), "  AB12345 ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 GHI67890" );

    ret = merge_series( outTape, inTape, inTape2 );
    outTape.flush();

    EXPECT_TRUE( ret );
    EXPECT_EQ( ssOutTape->str(), "  AB12345 ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 GHI67890  AB09876  CD67890  EF12345  GH12345  GH67890" );
}

TEST_F( SortTest, GIVENsmallFileStreamWHENsortFileTHENproperlySorted ) {
    ASSERT_TRUE( PrepareSmallSet() );

    auto ret = sort_file( FILE_NAME, MAX_RECORDS_ON_PAGE, RECORD_SIZE );

    std::ifstream file( FILE_NAME );
    ASSERT_TRUE( file.good() );
    std::string result;
    result.assign( std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>() );
    file.close();

    EXPECT_EQ( result, sorted );
}

TEST_F( SortTest, GIVENbigFileStreamWHENsortFileTHENproperlySorted ) {
    ASSERT_TRUE( PrepareBigSet() );

    auto ret = sort_file( FILE_NAME, MAX_RECORDS_ON_PAGE, RECORD_SIZE );

    std::ifstream file( FILE_NAME );
    ASSERT_TRUE( file.good() );
    std::string result;
    result.assign( std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>() );
    file.close();

    EXPECT_EQ( result, sorted );
}

TEST_F( SortTest, GIVENsortedFileStreamWHENsortFileTHENproperlySorted ) {
    ASSERT_TRUE( PrepareSortedSet() );

    auto ret = sort_file( FILE_NAME, MAX_RECORDS_ON_PAGE, RECORD_SIZE );

    std::ifstream file( FILE_NAME );
    ASSERT_TRUE( file.good() );
    std::string result;
    result.assign( std::istreambuf_iterator<char>( file ), std::istreambuf_iterator<char>() );
    file.close();

    EXPECT_EQ( result, sorted );
}
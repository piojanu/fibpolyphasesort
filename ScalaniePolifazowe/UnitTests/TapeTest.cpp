#include "gtest/gtest.h"
#include "Tape.h"

#include <sstream>
#include <memory>

using namespace bfs;

struct InTapeTest : public ::testing::Test {
    InTapeTest() : sstape( new std::stringstream() ), inTape( std::dynamic_pointer_cast< std::iostream >( sstape ), 3, 9 ) {}

    std::shared_ptr<std::stringstream> sstape;
    InputTape inTape;
};

struct OutTapeTest : public ::testing::Test {
    OutTapeTest() : sstape( new std::stringstream() ), outTape( std::dynamic_pointer_cast< std::iostream >( sstape ), 3, 9 ) {}

    std::shared_ptr<std::stringstream> sstape;
    OutputTape outTape;
};

struct TapeTest : public ::testing::Test {
    TapeTest() : ssOutTape( new std::stringstream() ), ssInTape( new std::stringstream() ),
        outTape( std::dynamic_pointer_cast< std::iostream >( ssOutTape ), 3, 9 ), inTape( std::dynamic_pointer_cast< std::iostream >( ssInTape ), 3, 9 ) {};

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
        AddToInTapeStream( "CD67890" );
        AddToInTapeStream( "EF12345" );
        AddToInTapeStream( "GH67890" );
    }

    std::shared_ptr<std::stringstream> ssOutTape, ssInTape;
    OutputTape outTape;
    InputTape inTape;

private:
    inline void AddToInTapeStream( std::string elem ) {
        *ssInTape << std::setw( inTape.RECORD_SIZE ) << elem;
    }
};

struct CastTapeTest : public ::testing::Test {
    CastTapeTest() : ssOutTape( new std::stringstream() ), ssInTape( new std::stringstream() ),
        outTape( std::dynamic_pointer_cast< std::iostream >( ssOutTape ), 3, 9 ), inTape( std::dynamic_pointer_cast< std::iostream >( ssInTape ), 3, 9 ) {};

    std::shared_ptr<std::stringstream> ssOutTape, ssInTape;
    OutputTape outTape;
    InputTape inTape;
};

TEST_F( InTapeTest, GIVENemptyFileForInputTapeWHENgetAndPopFromTapeTHENemptyElementsReturned ) {
    sstape->str( " " );

    EXPECT_EQ( inTape.get(), "" );
    EXPECT_EQ( inTape.pop(), "" );
}

TEST_F( InTapeTest, GIVENfullFileForInputTapeWHENpopFromTapeTHENproperElementsReturned ) {
    sstape->str( "  AB12345  CD67890  EF12345  GH67890" );

    EXPECT_EQ( inTape.pop(), "AB12345" );
    EXPECT_EQ( inTape.pop(), "CD67890" );
    EXPECT_EQ( inTape.pop(), "EF12345" );
    EXPECT_EQ( inTape.pop(), "GH67890" );
    EXPECT_EQ( inTape.pop(), "" );
}

TEST_F( InTapeTest, GIVENfullFileForInputTapeWHENassignToDifferentInTapeAndPopFromTapeTHENproperElementsReturned ) {
    sstape->str( "  AB12345  CD67890  EF12345" );

    EXPECT_EQ( inTape.pop(), "AB12345" );

    InputTape diffInTape( std::make_shared<std::stringstream>( std::stringstream( "" ) ), inTape.MAX_RECORDS_ON_PAGE, inTape.RECORD_SIZE );
    diffInTape = std::move( inTape );

    EXPECT_EQ( diffInTape.pop(), "CD67890" );
    EXPECT_EQ( diffInTape.pop(), "EF12345" );
    EXPECT_EQ( inTape.pop(), "" );
}

TEST_F( InTapeTest, GIVENfullFileForInputTapeWHENgetAndPopFromTapeTHENproperElementsReturned ) {
    sstape->str( "  AB12345  CD67890  EF12345" );

    EXPECT_EQ( inTape.get(), "AB12345" );
    EXPECT_EQ( inTape.pop(), "AB12345" );

    EXPECT_EQ( inTape.get(), "CD67890" );
    EXPECT_EQ( inTape.pop(), "CD67890" );
    EXPECT_EQ( inTape.pop(), "EF12345" );

    EXPECT_EQ( inTape.get(), "" );
}

TEST_F( OutTapeTest, GIVENemptyOutputTapeWHENputOnTapeSomeElementsAndOneEmptyTHENelementsAreInFile ) {
    outTape.push( "AB12345" );
    outTape.push( "" );
    outTape.push( "FG12345" );
    outTape.push( "HI67890" );

    EXPECT_EQ( sstape->str(), "  AB12345  FG12345  HI67890" );
}

TEST_F( OutTapeTest, GIVENemptyOutputTapeWHENputOnTapeFullPageTHENelementsAreInFile ) {
    outTape.push( "AB12345" );
    outTape.push( "CDE67890" );
    outTape.push( "FG12345" );
    // Flush to file should happen here. Fourth element is added to new empty page.
    outTape.push( "HI67890" );

    EXPECT_EQ( sstape->str(), "  AB12345 CDE67890  FG12345" );
}

TEST_F( OutTapeTest, GIVENemptyOutputTapeWHENassignToDifferentOutTapeAndPutOnTapeFullPageTHENelementsAreInFile ) {
    outTape.push( "AB12345" );
    outTape.push( "CDE67890" );

    OutputTape diffOutTape( std::make_shared<std::stringstream>( std::stringstream( "" ) ), outTape.MAX_RECORDS_ON_PAGE, outTape.RECORD_SIZE );
    diffOutTape = std::move( outTape );

    diffOutTape.push( "FG12345" );

    EXPECT_EQ( sstape->str(), "  AB12345 CDE67890  FG12345" );
}

TEST_F( OutTapeTest, GIVENemptyOutputTapeWHENputOnTapeAndCastTHENpopProperElements ) {
    outTape.push( "AB12345" );
    outTape.push( "CD67890" );
    outTape.push( "EF12345" );

    InputTape inputTape( std::move( static_cast< Tape& >( outTape ) ) );

    EXPECT_EQ( inputTape.pop(), "AB12345" );
    EXPECT_EQ( inputTape.pop(), "CD67890" );
    EXPECT_EQ( inputTape.pop(), "EF12345" );
}

TEST_F( OutTapeTest, GIVENemptyOutputTapeWHENputOnTapeAndCastTHENpopProperElementsAndCurrentLoadedPageIsLost ) {
    outTape.push( "AB12345" );
    outTape.push( "CD67890" );
    outTape.push( "EF12345" );
    // Last is lost during cast
    outTape.push( "GH67890" );

    InputTape inputTape( std::move( static_cast< Tape& >( outTape ) ) );

    EXPECT_EQ( inputTape.pop(), "AB12345" );
    EXPECT_EQ( inputTape.pop(), "CD67890" );
    EXPECT_EQ( inputTape.pop(), "EF12345" );
    // Last is lost during cast
    EXPECT_NE( inputTape.pop(), "GH67890" );
}

TEST_F( OutTapeTest, GIVENoutputTapeWHENsetEmptySeriesCountTHENgetProperEmptySeriesCount ) {
    outTape.setEmptySeriesCount( 7 );

    EXPECT_EQ( outTape.getEmptySeriesCount(), 7 );
}

TEST_F( TapeTest, GIVENemptyOutputTapeWHENputSingleElementAndFlushTHENproperElementsInFile ) {
    outTape.push( "AB12345" );
    outTape.flush();

    EXPECT_EQ( ssOutTape->str(), "  AB12345" );
}

TEST_F( TapeTest, GIVENfullInputTapeWHENoutputTapePullSeriesTHENoutputTapeHasProperSeries ) {
    outTape.pullSeries( inTape );
    outTape.flush();

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345" );
    EXPECT_EQ( outTape.getSeriesCount(), 1 );

    outTape.pullSeries( inTape );
    outTape.flush();

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345  AB12345 GHI67890 TUW12345 XYZ67890" );
    EXPECT_EQ( outTape.getSeriesCount(), 2 );
}

TEST_F( TapeTest, GIVENfullInputTapeWHENoutputTapePullSeriesTHENproperSpliceTwoSeries ) {
    outTape.pullSeries( inTape );
    outTape.flush();

    ASSERT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345" );
    ASSERT_EQ( outTape.getSeriesCount(), 1 );
    ASSERT_EQ( inTape.pop(), "AB12345" );

    outTape.pullSeries( inTape );
    outTape.flush();

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 GHI67890 TUW12345 XYZ67890" );
    EXPECT_EQ( outTape.getSeriesCount(), 1 );

    outTape.pullSeries( inTape );
    outTape.flush();

    EXPECT_EQ( ssOutTape->str(), " ABC12345 ABC67890 DEF12345 DEF67890 GHI12345 GHI67890 TUW12345 XYZ67890  CD67890  EF12345  GH67890" );
    EXPECT_EQ( outTape.getSeriesCount(), 2 );
}

TEST_F( CastTapeTest, GIVENputOnOutputTapeWHENcastToInputTapeTHENproperPop ) {
    outTape.push( "AB12345" );
    outTape.push( "CD67890" );
    outTape.push( "EF12345" );
    outTape.push( "GH67890" );
    outTape.flush();

    InputTape castInTape = make_input( std::move( outTape ) );

    EXPECT_EQ( castInTape.pop(), "AB12345" );
    EXPECT_EQ( castInTape.pop(), "CD67890" );
    EXPECT_EQ( castInTape.pop(), "EF12345" );
    EXPECT_EQ( castInTape.pop(), "GH67890" );
}

TEST_F( CastTapeTest, GIVENputOnCastedInputTapeWHENcastToInputTapeTHENproperPop ) {
    outTape.push( "AB12345" );
    outTape.push( "CD67890" );
    outTape.flush();

    InputTape castInTape = make_input( std::move( outTape ) );

    // Loaded whole page during pop.
    EXPECT_EQ( castInTape.pop(), "AB12345" );

    // Lost CD67890 element.
    OutputTape castOutTape = make_output( std::move( castInTape ) );

    castOutTape.push( "EF12345" );
    castOutTape.push( "GH67890" );
    castOutTape.flush();

    castInTape = make_input( std::move( castOutTape ) );

    EXPECT_EQ( castInTape.pop(), "EF12345" );
    EXPECT_EQ( castInTape.pop(), "GH67890" );
}
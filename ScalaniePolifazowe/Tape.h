#pragma once
#include <iostream>
#include <sstream>
#include <cassert>
#include <iomanip>
#include <memory>

namespace bfs
{

struct SortStats {
    uint64_t reads = 0;
    uint64_t writes = 0;
    uint64_t phases = 0;

    void reset() {
        reads = 0;
        writes = 0;
        phases = 0;
    }
};

class Tape {
public:
    Tape( const Tape& ) = delete;
    virtual ~Tape() = default;

    // Flushes pageStream to end of fileStream.
    void flush() {
        m_fileStream->write( m_pageStream->str().c_str(), m_pageStream->str().length() );
        if ( m_sortStats ) {
            m_sortStats->writes++;
        }
        m_pageStream->clear();
        m_pageStream->str( "" );
    }

    std::shared_ptr<std::iostream> getFileStream() {
        return m_fileStream;
    }

    const uint64_t MAX_RECORDS_ON_PAGE;
    const uint64_t RECORD_SIZE;

protected:
    // Throws std::string if max_records_on_page <= 0
    Tape( std::shared_ptr<std::iostream> stream, const uint64_t &max_records_on_page, const uint64_t & record_size, std::shared_ptr<SortStats> sort_stats ) : m_fileStream( std::move( stream ) ), m_pageStream( new std::stringstream() ),
        m_sortStats( sort_stats ), MAX_RECORDS_ON_PAGE( max_records_on_page ), RECORD_SIZE( record_size ) {
        if ( max_records_on_page <= 0 ) {
            throw std::string( "max_records_on_page have to be greater then 0!" );
        }
    }
    // WARNING! Keep in mind that loaded page will be lost!
    Tape( Tape &&other ) : m_pageStream( new std::stringstream() ), MAX_RECORDS_ON_PAGE( other.MAX_RECORDS_ON_PAGE ), RECORD_SIZE( other.RECORD_SIZE ) {
        std::swap( m_fileStream, other.m_fileStream );
    }

    std::shared_ptr<std::iostream> m_fileStream;
    std::shared_ptr<SortStats> m_sortStats;
    std::unique_ptr<std::stringstream> m_pageStream;
};

class InputTape : public Tape {
public:
    InputTape( std::shared_ptr<std::iostream> stream, const uint64_t &max_records_on_page, const uint64_t & record_size, std::shared_ptr<SortStats> sort_stats = nullptr ) : Tape( std::move( stream ), max_records_on_page, record_size, std::move( sort_stats ) ), m_currentElem( "" ) {}
    InputTape( Tape &&tape ) : Tape( std::move( tape ) ), m_currentElem( "" ) {}
    InputTape( InputTape &&other ) : Tape( std::move( other.m_fileStream ), other.MAX_RECORDS_ON_PAGE, other.RECORD_SIZE, std::move( other.m_sortStats ) ), m_currentElem( std::move( other.m_currentElem ) ) {
        std::swap( m_pageStream, other.m_pageStream );
    }
    InputTape( const InputTape& ) = delete;

    virtual ~InputTape() = default;

    // MAX_RECORDS_ON_PAGE has to match!
    // Throws std::string if max_records_on_page doesn't match.
    InputTape& operator= ( InputTape &&other );

    // Pops and returns first element from tape.
    std::string pop();
    // Returns first element, but doesn't pull it from tape.
    std::string get();

private:
    bool fetch();

    std::string m_currentElem;
};

class OutputTape : public Tape {
public:
    OutputTape( std::shared_ptr<std::iostream> stream, const uint64_t &max_records_on_page, const uint64_t & record_size, std::shared_ptr<SortStats> sort_stats = nullptr ) : Tape( std::move( stream ), max_records_on_page, record_size, std::move( sort_stats ) ), m_seriesCount( 0 ), m_emptySeriesCount( 0 ), m_lastElem( "" ) {}
    OutputTape( Tape &&tape ) : Tape( std::move( tape ) ), m_seriesCount( 0 ), m_emptySeriesCount( 0 ), m_lastElem( "" ) {}
    OutputTape( OutputTape &&other ) : Tape( std::move( other.m_fileStream ), other.MAX_RECORDS_ON_PAGE, other.RECORD_SIZE, std::move( other.m_sortStats ) ), m_seriesCount( other.m_seriesCount ), m_emptySeriesCount( 0 ), m_lastElem( std::move( other.m_lastElem ) ) {
        std::swap( m_pageStream, other.m_pageStream );
    }
    OutputTape( const OutputTape& ) = delete;

    virtual ~OutputTape() = default;

    // MAX_RECORDS_ON_PAGE has to match!
    // Throws std::string if max_records_on_page doesn't match.
    OutputTape& operator= ( OutputTape &&other );

    // Also counts series.
    // Doesn't add empty elements.
    void push( std::string elem );

    // Pull whole series from input tape. Count pulled series and take care of splice effect.
    // Returns true if successfully pulled series and false if inTape didn't returned any element.
    bool pullSeries( InputTape &inTape );

    uint64_t getSeriesCount() const {
        return m_seriesCount;
    }

    uint64_t getEmptySeriesCount() const {
        return m_emptySeriesCount;
    }

    void setEmptySeriesCount( uint64_t count ) {
        m_emptySeriesCount = count;
    }

private:
    uint64_t m_seriesCount;
    uint64_t m_emptySeriesCount;
    std::string m_lastElem;
};

// It is safe, as loaded page will be flushed before cast.
// Note that series mechanism is initialized from scratch in ctor of OutputTape.
inline InputTape make_input( OutputTape &&outTape ) {
    outTape.flush();
    InputTape inputTape( std::move( static_cast< Tape& >( outTape ) ) );
    return inputTape;
}

// WARNING! Keep in mind that loaded page and current element will be lost!
inline OutputTape make_output( InputTape &&inTape ) {
    OutputTape outputTape( std::move( static_cast< Tape& >( inTape ) ) );
    return outputTape;
}

} //namespace

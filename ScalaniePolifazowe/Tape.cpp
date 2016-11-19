#include "Tape.h"

bool bfs::InputTape::fetch() {
    if ( ( m_pageStream->str().empty() || m_pageStream->eof() ) && !m_fileStream->eof() ) {
        char *cstr = new char[ MAX_RECORDS_ON_PAGE * RECORD_SIZE ];
        std::memset( cstr, ' ', MAX_RECORDS_ON_PAGE * RECORD_SIZE );
        m_fileStream->read( cstr, MAX_RECORDS_ON_PAGE * RECORD_SIZE );
        if ( m_sortStats ) {
            m_sortStats->reads++;
        }
        if ( m_fileStream->eof() ) {
            m_fileStream->clear();
        }
        m_pageStream->clear();
        m_pageStream->str( std::string( cstr, MAX_RECORDS_ON_PAGE * RECORD_SIZE ) );
        delete[] cstr;
    }

    if ( !( m_pageStream->str().empty() || m_pageStream->eof() ) ) {
        *m_pageStream >> m_currentElem;
        return true;
    } else {
        m_currentElem = std::string( "" );
        return false;
    }
}

bfs::InputTape& bfs::InputTape::operator= ( InputTape &&other ) {
    if ( this != &other && MAX_RECORDS_ON_PAGE == other.MAX_RECORDS_ON_PAGE ) {
        std::swap( m_fileStream, other.m_fileStream );
        std::swap( m_pageStream, other.m_pageStream );
        std::swap( m_currentElem, other.m_currentElem );
    } else {
        throw std::string( "max_records_on_page doesn't match!" );
    }

    return *this;
}

std::string bfs::InputTape::pop() {
    if ( m_currentElem.empty() && !this->fetch() ) {
        return std::string( "" );
    }

    std::string elem = std::move( m_currentElem );
    this->fetch();
    return elem;
}

std::string bfs::InputTape::get() {
    if ( m_currentElem.empty() && !this->fetch() ) {
        return std::string( "" );
    }

    return m_currentElem;
}

bfs::OutputTape& bfs::OutputTape::operator= ( OutputTape &&other ) {
    if ( this != &other && MAX_RECORDS_ON_PAGE == other.MAX_RECORDS_ON_PAGE ) {
        std::swap( m_fileStream, other.m_fileStream );
        std::swap( m_pageStream, other.m_pageStream );
        std::swap( m_seriesCount, other.m_seriesCount );
        std::swap( m_lastElem, other.m_lastElem );
    } else {
        throw std::string( "max_records_on_page doesn't match!" );
    }

    return *this;
}

void bfs::OutputTape::push( std::string elem ) {
    if ( elem.empty() ) {
        return;
    }

    *m_pageStream << std::setw( RECORD_SIZE ) << std::move( elem );

    if ( m_pageStream->str().length() == MAX_RECORDS_ON_PAGE * RECORD_SIZE ) {
        m_fileStream->write( m_pageStream->str().c_str(), MAX_RECORDS_ON_PAGE * RECORD_SIZE );
        if ( m_sortStats ) {
            m_sortStats->writes++;
        }
        m_pageStream->clear();
        m_pageStream->str( "" );
    }

    assert( m_pageStream->str().length() <= MAX_RECORDS_ON_PAGE * RECORD_SIZE );
}

bool bfs::OutputTape::pullSeries( InputTape &inTape ) {
    std::string elem = inTape.get();
    if ( elem.empty() ) {
        return false;
    } else if ( !m_lastElem.empty() && m_lastElem <= elem ) {
        m_seriesCount--;
    }
    this->push( elem );
    m_lastElem = std::move( elem );
    inTape.pop();

    while ( true ) {
        elem = inTape.get();
        if ( elem.empty() || m_lastElem > elem ) {
            m_seriesCount++;
            return true;
        }
        this->push( elem );
        m_lastElem = std::move( elem );
        inTape.pop();
    }
}

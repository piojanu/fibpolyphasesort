#include "bigfilesort.h"

#include <fstream>
#include <map>

bool bfs::distribute_series( InputTape &inTape, OutputTape &outTape, OutputTape &outTape2 ) {
    if ( outTape.getSeriesCount() != 0 || outTape2.getSeriesCount() != 0 ) {
        return false;
    }

    if ( outTape.pullSeries( inTape ) ) {
        auto run = true;
        while ( run ) {
            auto firstRun = true;
            for ( auto i = outTape.getSeriesCount() + outTape2.getSeriesCount(); i != outTape2.getSeriesCount(); firstRun = false ) {
                if ( !outTape2.pullSeries( inTape ) ) {
                    run = false;
                    // If it is first run, there is no need to set empty series. Series counts are still Fibonacci numbers.
                    if ( !firstRun ) {
                        outTape2.setEmptySeriesCount( i - outTape2.getSeriesCount() );
                    }
                    break;
                }
            }

            if ( run != true ) {
                break;
            }

            firstRun = true;
            for ( auto i = outTape.getSeriesCount() + outTape2.getSeriesCount(); i != outTape.getSeriesCount(); firstRun = false ) {
                if ( !outTape.pullSeries( inTape ) ) {
                    run = false;
                    // If it is first run, there is no need to set empty series. Series counts are still Fibonacci numbers.
                    if ( !firstRun ) {
                        outTape.setEmptySeriesCount( i - outTape2.getSeriesCount() );
                    }
                    break;
                }
            }
        }

        // WA for finding sorted files.
        if ( outTape.getSeriesCount() == 1 && outTape2.getSeriesCount() == 0 ) {
            outTape2.setEmptySeriesCount( 1 );
        }

        return true;
    } else {
        return false;
    }
}

bool bfs::merge_series( OutputTape &outTape, InputTape &inTape, InputTape &inTape2 ) {
    if ( inTape.get().empty() || inTape2.get().empty() ) {
        return false;
    }

    std::string prevElem[] = { inTape.get() , inTape2.get() };

    do {
        if ( ( inTape2.get().empty() || inTape2.get() < prevElem[ 1 ] || inTape.get() <= inTape2.get() ) && inTape.get() >= prevElem[ 0 ] ) {
            prevElem[ 0 ] = inTape.get();
            outTape.push( inTape.pop() );
        } else if ( inTape2.get() >= prevElem[ 1 ] ) {
            prevElem[ 1 ] = inTape2.get();
            outTape.push( inTape2.pop() );
        }
    } while ( inTape.get() >= prevElem[ 0 ] || inTape2.get() >= prevElem[ 1 ] );

    return true;
}

bool bfs::sort_file( const std::string &filePath, const uint64_t maxRecordsOnPage, const uint64_t recordSize, std::shared_ptr<SortStats> sortStats ) {
    const std::string BUFF_ONE = "buffOne", BUFF_TWO = "buffTwo";

    std::shared_ptr<std::fstream> file( new std::fstream( filePath, std::ios::in ) ), buffOne( new std::fstream( BUFF_ONE, std::ios::out | std::ios::trunc ) ), buffTwo( new std::fstream( BUFF_TWO, std::ios::out | std::ios::trunc ) );
    if ( !( file->good() && buffOne->good() && buffTwo->good() ) ) {
        return false;
    }

    std::map<std::fstream*, std::string> fileNames;
    fileNames[ file.get() ] = filePath;
    fileNames[ buffOne.get() ] = BUFF_ONE;
    fileNames[ buffTwo.get() ] = BUFF_TWO;

    InputTape inTape( std::static_pointer_cast< std::iostream >( file ), maxRecordsOnPage, recordSize, sortStats );
    OutputTape outTape( std::static_pointer_cast< std::iostream >( buffOne ), maxRecordsOnPage, recordSize, sortStats ),
        outTape2( std::static_pointer_cast< std::iostream >( buffTwo ), maxRecordsOnPage, recordSize, sortStats );

    // Distribute, flash output tapes and save empty series count
    distribute_series( inTape, outTape, outTape2 );
    outTape.flush();
    outTape2.flush();
    auto emptySeriesOutTape = outTape.getEmptySeriesCount();
    auto emptySeriesOutTape2 = outTape2.getEmptySeriesCount();

    // Reopen files
    file->close();
    file->open( filePath, std::ios::out | std::ios::trunc );
    buffOne->close();
    buffOne->open( BUFF_ONE, std::ios::in );
    buffTwo->close();
    buffTwo->open( BUFF_TWO, std::ios::in );
    if ( !( file->good() && buffOne->good() && buffTwo->good() ) ) {
        return false;
    }

    // Change tapes
    outTape = OutputTape( std::static_pointer_cast< std::iostream >( file ), maxRecordsOnPage, recordSize, sortStats );
    inTape = InputTape( std::static_pointer_cast< std::iostream >( buffOne ), maxRecordsOnPage, recordSize, sortStats );
    InputTape inTape2 = InputTape( std::static_pointer_cast< std::iostream >( buffTwo ), maxRecordsOnPage, recordSize, sortStats );

    // If sorted at the beggining
    if ( inTape.get().empty() && inTape2.get().empty() ) {
        return true;
    }

    if ( emptySeriesOutTape != 0 ) {
        for ( auto i = emptySeriesOutTape; i > 0; i-- ) {
            outTape.pullSeries( inTape2 );
        }
    } else if ( emptySeriesOutTape2 != 0 ) {
        for ( auto i = emptySeriesOutTape2; i > 0; i-- ) {
            outTape.pullSeries( inTape );
        }
    }

    while ( true ) {
        while ( merge_series( outTape, inTape, inTape2 ) );
        if ( sortStats ) {
            sortStats->phases++;
        }
        outTape.flush();

        if ( inTape.get().empty() && inTape2.get().empty() ) {
            auto outStream = std::static_pointer_cast< std::fstream >( outTape.getFileStream() );
            if ( fileNames[ outStream.get() ] != filePath ) {
                outStream->close();
                outStream->open( fileNames[ outStream.get() ], std::ios::in );
                file->close();
                file->open( filePath, std::ios::out | std::ios::trunc );

                std::string str;
                std::getline( *outStream, str );
                *file << str;
            }

            file->close();
            buffOne->close();
            buffTwo->close();

            std::remove( BUFF_ONE.c_str() );
            std::remove( BUFF_TWO.c_str() );

            return true;
        }

        if ( inTape.get().empty() ) {
            auto newOutBuff = std::static_pointer_cast< std::fstream >( inTape.getFileStream() );
            auto newInBuff = std::static_pointer_cast< std::fstream >( outTape.getFileStream() );

            newOutBuff->close();
            newOutBuff->open( fileNames[ newOutBuff.get() ], std::ios::out | std::ios::trunc );
            newInBuff->close();
            newInBuff->open( fileNames[ newInBuff.get() ], std::ios::in );
            if ( !( newOutBuff->good() && newInBuff->good() ) ) {
                return false;
            }

            outTape = OutputTape( std::static_pointer_cast< std::iostream >( newOutBuff ), maxRecordsOnPage, recordSize, sortStats );
            inTape = InputTape( std::static_pointer_cast< std::iostream >( newInBuff ), maxRecordsOnPage, recordSize, sortStats );
        } else {
            auto newOutBuff = std::static_pointer_cast< std::fstream >( inTape2.getFileStream() );
            auto newInBuff = std::static_pointer_cast< std::fstream >( outTape.getFileStream() );

            newOutBuff->close();
            newOutBuff->open( fileNames[ newOutBuff.get() ], std::ios::out | std::ios::trunc );
            newInBuff->close();
            newInBuff->open( fileNames[ newInBuff.get() ], std::ios::in );
            if ( !( newOutBuff->good() && newInBuff->good() ) ) {
                return false;
            }

            outTape = OutputTape( std::static_pointer_cast< std::iostream >( newOutBuff ), maxRecordsOnPage, recordSize, sortStats );
            inTape2 = InputTape( std::static_pointer_cast< std::iostream >( newInBuff ), maxRecordsOnPage, recordSize, sortStats );
        }
    }
}

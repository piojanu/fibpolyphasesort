#pragma once
#include "Tape.h"

#include <iostream>

namespace bfs
{

// Does Fibonacci distribution on three tapes.
// outTape and outTape2 series count have to be 0.
// Returns false if above isn't true or if first pullSeries failed (e.g. inTape is empty).
// Otherwise returns true.
bool distribute_series( InputTape &inTape, OutputTape &outTape, OutputTape &outTape2 );

// Merges series from two input tapes into outtape.
// Returns false if one or both input tapes are empty.
// Otherwise returns true.
bool merge_series( OutputTape &outTape, InputTape &inTape, InputTape &inTape2 );

// Returns pointer to sorted file (this can be a one of help buffers!).
bool sort_file( const std::string &filePath, const uint64_t maxRecordsOnPage, const uint64_t recordSize, std::shared_ptr<SortStats> sortStats = nullptr );

} //namespace
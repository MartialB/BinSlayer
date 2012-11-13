/**
 * \file Hungarian.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include <iomanip>
#include <stdlib.h>
#include <list>
#include "Hungarian.hh"
#include "BinSlayer_Exception.hh"

BinSlay::Hungarian::Hungarian(
	BinSlay::Hungarian::Cost *matrix,
	size_t m_size,
	bool search_uncovered_zeros_by_col
)
  :  _cost_matrix_size(m_size),
     _matrix(matrix),
     _z0_row(0),
     _z0_col(0),
     _last_visited(0),
     _recently_uncovered(0),
     _search_uncovered_zeros_by_col(true)
{
  _last_visited = 0;
  _recently_uncovered = -1;

  try
    {
      // We create four arrays to cache various data needed by the algorithm in order
      // to avoid iterating over the full matrix, which it is very expensive.
      //
      //  - '_state_row': cache the status of each row -> cover ? uncover ?
      //  - '_state_row': cache the status of each column -> cover ? uncover ?
      //  - '_starred_col': cache the number of starred zeros of each column.
      //  - '_primed_col': cache the number of primed zeros of each column.
      _state_row = new bool[_cost_matrix_size];
      _state_col = new bool[_cost_matrix_size];
      _starred_col = new unsigned int[_cost_matrix_size];
      _primed_col = new unsigned int[_cost_matrix_size];
      // Intialisation
      for (size_t i = 0; i < _cost_matrix_size; ++i) {
	_state_row[i] = false;
	_state_col[i] = false;
	_starred_col[i] = 0;
	_primed_col[i] = 0;
      }
    }
  catch (std::bad_alloc &exception)
    {
      std::stringstream error_msg("Critical: ");
      error_msg << exception.what() << " in file " << __FILE__
		<<  " in function " << __FUNCTION__ << " at line " << __LINE__ << ".";
      throw BinSlay::Error(error_msg.str());
    }

  // This paremeter is currently unused.
  this->_search_uncovered_zeros_by_col = search_uncovered_zeros_by_col;
}

BinSlay::Hungarian::~Hungarian()
{
  delete [] _state_row;
  delete [] _state_col;
  delete [] _starred_col;
  delete [] _primed_col;
}


void BinSlay::Hungarian::solve()
{
  // std::cout << "this->_cost_matrix.size() = " << _cost_matrix_size << std::endl;
  // for (unsigned int i = 0; i < _cost_matrix_size; i++)
  // for(unsigned int j = 0; j < _cost_matrix_size; j++)
  // {
  //   // Worst case (to test the speed of our implementation of the algorithm)
  //   this->m[i * _cost_matrix_size + j]._cost = i*j;
  // }
  // if (_cost_matrix_size < 50)
  //   {
  //     std::cout << "Solving assignment problem!" << std::endl;
  //     std::cout << std::endl;
  //     this->display();
  //     std::cout << std::endl;
  //   }

  bool isFinished = false;
  int step = 0;

#ifdef MEASURE_PERFORMANCE
  std::cerr << "Size of the cost matrix = " << _cost_matrix_size << std::endl;
#endif // !MEASURE_PERFORMANCE

  // This is a step-by-step algorithm
  while (!isFinished)
    {
      switch (step)
	{
	case 0:
	  _step0(step);
	  break;
	case 1:
	  _step1(step);
	  break;
	case 2:
	  _step2(step);
	  break;
	case 3:
	  _step3(step);
	  break;
	case 4:
	  _step4(step);
	  break;
	case 5:
	  isFinished = true;
	  break;
	}
    }

  // if (_cost_matrix_size < 50)
  //   {
  //     std::cout << std::endl;
  //     this->display();
  //     std::cout << std::endl;
  //     std::cout << "Solution found!" << std::endl;
  //   }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

void BinSlay::Hungarian::_step0(int &step)
{
  //
  // STEP0: Rows reduction + Columns reduction + starred zero
  //

  // For each row of the cost matrix, find the smallest element and substract
  // it from every element in tis row.Rows reduction
  for (size_t row = 0; row < _cost_matrix_size; ++row) {
    unsigned int min = -1;
    for (size_t col = 0; col < _cost_matrix_size; ++col)
      min = _matrix[row * _cost_matrix_size + col]._cost < min ?
	_matrix[row * _cost_matrix_size + col]._cost : min;
    for (size_t col = 0; col < _cost_matrix_size; ++col)
      _matrix[row * _cost_matrix_size + col]._cost -= min;
  }

  // Columns reduction
  for (size_t col = 0; col < this->_cost_matrix_size; col++) {
    unsigned int min = -1;
    for (size_t row = 0; row < this->_cost_matrix_size; row++)
      min = _matrix[row * _cost_matrix_size + col]._cost < min ?
	_matrix[row * _cost_matrix_size + col]._cost : min;
    for (size_t row = 0; row < this->_cost_matrix_size; row++)
      _matrix[row * _cost_matrix_size + col]._cost -= min;
  }

  // Starred zero
  for (size_t row = 0; row < this->_cost_matrix_size; row++)
    for (size_t col = 0; col < this->_cost_matrix_size; col++) {
      if (!_matrix[row * _cost_matrix_size + col]._cost) {

	// Check if there is a starred zero in its cols
	if (_starred_col[col])
	  goto check_next_zero;

	// Check if there is a starred zero in its rows
	for (unsigned int c = 0; c < this->_cost_matrix_size; c++)
	  if (!_matrix[row * _cost_matrix_size + c]._cost &&
	      _matrix[row * _cost_matrix_size + c]._starred)
	    goto check_next_zero;

	// Starred it if this zero is unique in its row and col
	this->_matrix[row * _cost_matrix_size + col]._starred = true;
	++_starred_col[col]; // __starred_col__cached
      }
    check_next_zero:
      ;
    }
  step = 1;
}

void BinSlay::Hungarian::_step1(int &step)
{
  // Cover all columns if they contain a starred zero
  for (size_t col = 0; col < this->_cost_matrix_size; ++col) {
    // If a column contains one or more primed zeros, erase them 
    if (_primed_col[col]) {
      for (size_t row = 0; row < this->_cost_matrix_size; ++row)
	this->_matrix[row * _cost_matrix_size + col]._primed = false;
      _primed_col[col] = 0;
    }
    // If the column contains at least one starred zero, cover it
    if (_starred_col[col]) { // __starred_col__cached
      // Cover it
      _state_col[col] = true;
    }
  }
  // Check if all columns are covered and if not go to the step2
  if (!_is_all_columns_covered()) {
    step = 2;
    return ;
  }
  // If yes, end of the algorithm, optimal solution found
  step = 5;
}

void BinSlay::Hungarian::_step2(int &step)
{
  // For all uncovered zeros
  bool isFinished = false;

  while (!isFinished)
    {
      unsigned int r, c, starred_zero_in_col = 0;

      // Find an uncovered zero
      if (!_find_uncovered_zero(r, c)) {
	// If not found, exit this loop and go to step4
	isFinished = true;
	break;
      }
      // Prime it
      this->_matrix[r * _cost_matrix_size + c]._primed = true;
      ++_primed_col[c];
      // If there is no starred zero in the row containing this primed zero,
      // go to step3
      if (!_is_starred_zero_in_row(r, starred_zero_in_col)) {
	//_last_visited = 0;
	// _recently_uncovered = 0;
	step = 3;
	this->_z0_row = r;
	this->_z0_col = c;
	return ;
      }
      // Else cover the current row of the primed zero and uncover the
      // column containing the starred zero
      _state_row[r] = true;
      _state_col[starred_zero_in_col] = false;
      _recently_uncovered = starred_zero_in_col;  // __uncovered__
    }  
  // If the cost matrix does not contain any uncovered zero, go to step4
  step = 4;
}

void BinSlay::Hungarian::_step3(int &step)
{
  bool isFinished = false;

  // Construct a series of alternating primed and starred zeros as follows:
  // Let Z0 represent the uncovered primed zero found in step2. Let Z1 denote
  // the starred zero in the column of Z0 (if any). Let Z2 denote the primed zero
  // in the row of Z1 (there will always be one). Continue until the series
  // terminates at a primed zero that has no starred zero in its column.
  std::list<std::pair<size_t, size_t> >	series;
  series.push_front(std::make_pair(this->_z0_row, this->_z0_col));

  while(!isFinished)
    {
      unsigned int row_of_starred_zero = 0;
      unsigned int col_of_primed_zero = 0;
      bool is_starred_zero_in_col = false;
      bool is_primed_zero_in_row = false;

      // Find a starred zero in the column if any
      is_starred_zero_in_col = _find_starred_zero_in_col(_z0_col, row_of_starred_zero);
      
      if (is_starred_zero_in_col/* && !m[row_of_starred_zero * _cost_matrix_size + _z0_col]._series*/)
	{
	  // add the starred zero to the series
	  series.push_front(std::make_pair(row_of_starred_zero, this->_z0_col));
	  // update row
	  this->_z0_row = row_of_starred_zero;
	  // add the primed zero to the series
	  is_primed_zero_in_row = _find_primed_zero_in_row(row_of_starred_zero, col_of_primed_zero);
	  series.push_front(std::make_pair(row_of_starred_zero, col_of_primed_zero));
	  // update col
	  this->_z0_col = col_of_primed_zero;
	}
      else
	isFinished = true;
      // G++ is happy :)))
      (void)is_primed_zero_in_row;
    }

  // Unstar each starred zero in the series and star each primed zero of the series
  // todo: keep col and row for each zero in the series to avoid looking at the whole
  // matrix one more time
  for (auto it = series.begin(); it != series.end(); ++it) {

    if (this->_matrix[it->first * _cost_matrix_size + it->second]._starred) {
      this->_matrix[it->first * _cost_matrix_size + it->second]._starred = false;
      if (_starred_col[it->second]) --_starred_col[it->second]; // __starred_col__cached
    }
    if (this->_matrix[it->first * _cost_matrix_size + it->second]._primed) {
      this->_matrix[it->first * _cost_matrix_size + it->second]._primed = false;
      if (_primed_col[it->second]) --_primed_col[it->second];

      this->_matrix[it->first * _cost_matrix_size + it->second]._starred = true;
      ++_starred_col[it->second]; // __starred_col__cached
    }
  }

  for (size_t idx = 0; idx < _cost_matrix_size; ++idx) {
    _state_row[idx] = false;
    _state_col[idx] = false; // __uncovered__
  }
  _last_visited = 0;
  // We report the erasing of primed zeros in step one

  // Go to step1
  step = 1;
}

void BinSlay::Hungarian::_step4(int &step)
{
  // Get the smallest uncovered element
  unsigned int min_uncovered_element = _get_min_uncovered_elem();

  // Add 'min_uncovered_element' to every element in covered rows
  for (size_t row = 0; row < _cost_matrix_size; ++row) {

    if (_state_row[row])
      for (size_t col = 0; col < _cost_matrix_size; ++col) {
	if (!this->_matrix[row * _cost_matrix_size + col]._cost)
	  {
	    this->_matrix[row * _cost_matrix_size + col]._primed = false;
	    if (_primed_col[col]) --_primed_col[col];

	    if (this->_matrix[row * _cost_matrix_size + col]._starred) {
	      this->_matrix[row * _cost_matrix_size + col]._starred = false;
	      if (_starred_col[col]) --_starred_col[col]; // __starred_col__cached
	    }
	  }
	this->_matrix[row * _cost_matrix_size + col]._cost += min_uncovered_element;
      }
  }
  // Substract 'min_uncovered_element' from every element in uncovered columns
  for (size_t col = 0; col < _cost_matrix_size; ++col) {
    if (!_state_col[col])
      for (size_t row = 0; row < _cost_matrix_size; ++row) {
	this->_matrix[row * _cost_matrix_size + col]._cost -= min_uncovered_element;
	// if (!this->m[row * _cost_matrix_size + col]._cost)
	//   _cached_zeros.insert(std::make_pair(row, col));
	// if (!this->m[row * _cost_matrix_size + col]._cost && !_state_row[row]) {
	//   _cached_zeros.push({row, col});
	// } 
      }
  }
  // Go to step2
  step = 2;
}

// Node: The order in which zeros are peeked can impact on the final mapping in case
// several edit paths have the same minimal edit cost.
bool BinSlay::Hungarian::_find_uncovered_zero(unsigned int &r, unsigned int &c)
{
  unsigned int nb_pass = 0;
 
 redo_check:

  // We start the looking at the last visited col to avoid looking
  // at col we already checked before.
  for (size_t col = _last_visited; col < _cost_matrix_size; ++col) {
    // If the column is covered we can skip it
    if (_state_col[col])
      continue;

    for (size_t row = 0; row < _cost_matrix_size; ++row) {
      if (!_state_row[row] && !_matrix[row * _cost_matrix_size + col]._cost) {
	r = row;
	c = col;
	// We cache the last column we visited
	_last_visited = col;
	return true;
      }
    }
  }

  // We increase the number of passe and we reset the _last_visited field to 0
  // in order to restart at the beginning of the matrix. Indeed, we do not start
  // at the first element but at the '_last_visited' col.
  ++nb_pass;
  _last_visited = 0;

  // If the number of pass > 1, this means that we check ALL the matrix
  // and we didn't find any uncovered zero.
  if (nb_pass == 1)
    goto redo_check;
  return false;
}

bool BinSlay::Hungarian::_is_starred_zero_in_row(
	unsigned int row,
	unsigned int &ret
) const
{
  bool is_starred_zero_in_row = false;

  for (unsigned int col = 0; col < this->_cost_matrix_size; ++col) {
    if (!this->_matrix[row * _cost_matrix_size + col]._cost && // _cost = 0
	this->_matrix[row * _cost_matrix_size + col]._starred) // _starred = true
      {
	is_starred_zero_in_row = true;
	ret = col;
	break;
      }
  }
  return is_starred_zero_in_row;
}

bool BinSlay::Hungarian::_find_starred_zero_in_col(
	unsigned int col,
	unsigned int &r
) const
{
  bool is_starred_zero_is_in_row = false;

  // If the column does not contain zero, we can skip it, just by checking
  // the state of the column - optimisation by caching information
  if (!_starred_col[col])
    return false;
  // Otherwise we need to iterate over the whole column
  for (size_t row = 0; row < this->_cost_matrix_size; ++row) {
    if (!this->_matrix[row * _cost_matrix_size + col]._cost && // _cost = 0
	this->_matrix[row * _cost_matrix_size + col]._starred) // _starred = true
      {
	is_starred_zero_is_in_row = true;
	r = row;
	break;
      }
  }
  return is_starred_zero_is_in_row;
}

bool BinSlay::Hungarian::_find_primed_zero_in_row(unsigned int row, unsigned int &c) const
{
  bool is_primed_zero_is_in_col = false;

  for (size_t col = 0; col < this->_cost_matrix_size; ++col) {

      if (!this->_matrix[row * _cost_matrix_size + col]._cost && // _cost = 0
	  this->_matrix[row * _cost_matrix_size + col]._primed) // _primed = true
	{
	  is_primed_zero_is_in_col = true;
	  c = col;
	  break;
	}
    }
  return is_primed_zero_is_in_col;
}

inline bool BinSlay::Hungarian::_is_row_covered(unsigned int row) const
{
  return _state_row[row];
}

inline bool BinSlay::Hungarian::_is_col_covered(unsigned int col) const
{
  return _state_col[col];
}

unsigned int BinSlay::Hungarian::_get_min_uncovered_elem() const
{
  unsigned int min_uncovered_element = -1;

  for (size_t row = 0; row < this->_cost_matrix_size; ++row) {
    if (_state_row[row])
      continue;
    for (size_t col = 0; col < this->_cost_matrix_size; ++col)
      if (!_state_col[col] && this->_matrix[row * _cost_matrix_size + col]._cost < min_uncovered_element)
	min_uncovered_element = this->_matrix[row * _cost_matrix_size + col]._cost;
  }
  return min_uncovered_element;
}

bool BinSlay::Hungarian::_is_all_columns_covered() const
{
  bool all_covered = true;

  for (size_t col = 0; col < this->_cost_matrix_size; ++col)
    if (!_state_col[col]) {
      all_covered = false;
      break;
    }
  return all_covered;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <iomanip>

void BinSlay::Hungarian::display() const
{
  for (size_t i = 0; i < _cost_matrix_size; ++i) {
    for (size_t j = 0; j < _cost_matrix_size; ++j) {
      if (this->_matrix[i* _cost_matrix_size +j]._cost < 4000000)
	std::cout << std::dec << std::setw(4) << std::right << std::setfill(' ')
		  << this->_matrix[i* _cost_matrix_size +j]._cost;
      else
	std::cout << std::setw(4) << std::right << std::setfill(' ') << "inf";	    
      if (this->_matrix[i* _cost_matrix_size +j]._starred)
	std::cout << std::setw(0) << std::internal << "*";
      if (this->_matrix[i* _cost_matrix_size +j]._primed)
	std::cout << std::setw(0) << std::internal << "'";
      std::cout << "   ";
    }
    std::cout << std::endl;
  }
}

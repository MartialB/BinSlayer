/**
 * \file Hungarian.cpp
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#include <iomanip>
#include <stdlib.h>
#include "Hungarian.hh"

BinSlay::Hungarian::Hungarian(BinSlay::Hungarian::Cost *m, unsigned int m_size,
			      bool search_uncovered_zeros_by_col)
  :  _cost_matrix_size(m_size),
    m(m)
{
  // srand(time(NULL));
  // _cost_matrix_size = 200;

  _last_visited = 0;
  _recently_uncovered = -1;

  _state_row = (bool*)malloc(sizeof(bool) * _cost_matrix_size);
  _state_col = (bool*)malloc(sizeof(bool) * _cost_matrix_size);
  _starred_col = (unsigned int*)malloc(sizeof(unsigned int) * _cost_matrix_size);

  for (unsigned int i = 0; i < _cost_matrix_size; ++i)
    {
      _state_row[i] = false;
      _state_col[i] = false;
      _starred_col[i] = 0;
    }

  this->_search_uncovered_zeros_by_col = search_uncovered_zeros_by_col;

  // std::cout << "this->_cost_matrix.size() = " << _cost_matrix_size << std::endl;
  // for (unsigned int i = 0; i < _cost_matrix_size; i++)
  //   for(unsigned int j = 0; j < _cost_matrix_size; j++)
  //     {
  // 	// Worst case (to test the speed of our implementation of the algorithm)
  // 	 	this->m[i * _cost_matrix_size + j]._cost = i*j;
  // 	//   this->_cost_matrix[i][j]._cost = random() % 100;
  // 	// // Only 0
  // 	//this->_cost_matrix[i][j]._cost = 0;
  // 	// // Only infinite
  // 	//  	this->_cost_matrix[i][j]._cost = -1;
  //     }
}

/**
 * \brief	Contructor per copy.
 *
 * \param[in]	o The other instance of Hungarian object we want to copy.
 */
BinSlay::Hungarian::Hungarian(BinSlay::Hungarian const &o)
  : _cost_matrix_size(o._cost_matrix_size),
    m(o.m)  
{
  _series.resize(_cost_matrix_size);
}

/**
 * \brief	Destructor : delete all allocated data.
 */
BinSlay::Hungarian::~Hungarian()
{
  free(_state_row);
  free(_state_col);
  free(_starred_col);
}

/**
 * \brief	Contructor per assignation
 */
// BinSlay::Hungarian& BinSlay::Hungarian::operator=(BinSlay::Hungarian const &o)
// {
  
// }

// BinSlay::Hungarian::Cost& BinSlay::Hungarian::Cost::operator=(BinSlay::Hungarian::Cost &o)
// {
//   return o;
// }

void BinSlay::Hungarian::solve()
{

  // if (_cost_matrix_size < 50)
  //   {
  //     std::cout << "Solving assignment problem!" << std::endl;
  //     std::cout << std::endl;
  //     this->display();
  //     std::cout << std::endl;
  //   }
  

  bool isFinished = false;
  int step = 0;

  // std::cout << "Size of the cost matrix = " << _cost_matrix_size << std::endl;
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

  //  std::cout << "_cost_matrix_size = " << _cost_matrix_size << std::endl;

  // Rows reduction
  for (unsigned int row = 0; row < _cost_matrix_size; ++row)
    {
      unsigned int min = -1;
      for (unsigned int col = 0; col < _cost_matrix_size; ++col)
	min = m[row * _cost_matrix_size + col]._cost < min ?
	  m[row * _cost_matrix_size + col]._cost : min;
      for (unsigned int col = 0; col < _cost_matrix_size; ++col)
	m[row * _cost_matrix_size + col]._cost -= min;
    }

  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;

  // Columns reduction
  for (unsigned int col = 0; col < this->_cost_matrix_size; col++)
    {
      unsigned int min = -1;
      for (unsigned int row = 0; row < this->_cost_matrix_size; row++)
  	min = this->m[row * _cost_matrix_size + col]._cost < min ?
	  this->m[row * _cost_matrix_size + col]._cost : min;
      for (unsigned int row = 0; row < this->_cost_matrix_size; row++)
  	this->m[row * _cost_matrix_size + col]._cost -= min;
    }

  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;

  // Starred zero
  for (unsigned int row = 0; row < this->_cost_matrix_size; row++)
    for (unsigned int col = 0; col < this->_cost_matrix_size; col++)
      {
	if (!this->m[row * _cost_matrix_size + col]._cost)
	  {
	    // Check if there is a starred zero in its cols
	    for (unsigned int r = 0; r < this->_cost_matrix_size; r++)
	      if (!this->m[r * _cost_matrix_size + col]._cost &&
		  this->m[r * _cost_matrix_size + col]._starred)
		goto check_next_zero;

	    // Check if there is a starred zero in its rows
	    for (unsigned int c = 0; c < this->_cost_matrix_size; c++)
	      if (!this->m[row * _cost_matrix_size + c]._cost &&
		  this->m[row * _cost_matrix_size + c]._starred)
		goto check_next_zero;

	    // Starred it if this zero is unique in its row and col
	    this->m[row * _cost_matrix_size + col]._starred = true;
	    ++_starred_col[col]; // __starred_col__cached
	  }
      check_next_zero:
	;
      }
  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;
  //std::cout << "End of step0!" << std::endl;
  step = 1;
}

void BinSlay::Hungarian::_step1(int &step)
{
  // std::cout << "BEFORE STEP1:" << std::endl;
  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;
  //  sleep(1);

  // Cover all columns if they contain a starred zero
  for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
    {
      if (_starred_col[col]) // __starred_col__cached
    	{
    	  _state_col[col] = true;
    	}
    }
    // for (unsigned int row = 0; row < this->_cost_matrix_size; ++row)
    //   {
    // 	// // Optimization: erase the prime zero here instead at the end of the
    // 	// // step 3
    // 	// if (this->m[row * _cost_matrix_size + col]._primed)
    // 	//   this->m[row * _cost_matrix_size + col]._primed = false;	

    // 	// Check if their is a starred zero in the current case
    // 	if (!this->m[row * _cost_matrix_size + col]._cost &&
    // 	    this->m[row * _cost_matrix_size + col]._starred)
    // 	  {
    // 	    std::cout << _starred_col[col] << std::endl;
    // 	    // Covered all the cases in the current column
    // 	    _state_col[col] = true;
    // 	    //this->cover_col(col);
    // 	    // Skip this column because it is already fully covered
	    
    // 	    break; //No we can't do this anymore now because we have to erase the prime zero !!!
    // 	  }
    //   }

  // Check if all columns are covered and if not go to the step2
  if (!_is_all_columns_covered())
    {
      step = 2;
      return ;
    }

  // If yes, end of the algorithm, optimal solution found
  step = 5;
}

void BinSlay::Hungarian::_step2(int &step)
{
  // std::cout << "BEFORE STEP2:" << std::endl;
  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;
  //  sleep(1);

  // For all uncovered zeros
  bool isFinished = false;

  while (!isFinished)
    {
      unsigned int r, c, starred_zero_in_col = 0;

      // Find an uncovered zero
      if (!_find_uncovered_zero(r, c))
	{
	  // If not found, exit this loop and go to step4
	  isFinished = true;
	  break;
	}

      // Prime it
      this->m[r * _cost_matrix_size + c]._primed = true;
      
      // If there is no starred zero in the row containing this primed zero,
      // go to step3
      if (!_is_starred_zero_in_row(r, starred_zero_in_col))
	{
	  // _last_visited = 0;
	  // _recently_uncovered = 0;
	  step = 3;
	  this->_z0_row = r;
	  this->_z0_col = c;
	  return ;
	}
      
      // Else cover the current row of the primed zero and uncover the
      // column containing the starred zero
      //this->cover_row(r);
      _state_row[r] = true;
      _state_col[starred_zero_in_col] = false;
      _recently_uncovered = starred_zero_in_col;  // __uncovered__
      // this->uncover_col(starred_zero_in_col);
    }
  
  // If the cost matrix does not contain any uncovered zero, go to step4
  // _last_visited = 0;
  // _recently_uncovered = 0;
  step = 4;
}

void BinSlay::Hungarian::_step3(int &step)
{
  // std::cout << "BEFORE STEP3:" << std::endl;
  // std::cout << std::endl;
  // this->display();
  // std::cout << std::endl;
  //sleep(1);

  bool isFinished = false;
  // unsigned int idx = 0;

  // Construct a series of alternating primed and starred zeros as follows:
  // Let Z0 represent the uncovered primed zero found in step2. Let Z1 denote
  // the starred zero in the column of Z0 (if any). Let Z2 denote the primed zero
  // in the row of Z1 (there will always be one). Continue until the series
  // terminates at a primed zero that has no starred zero in its column.
  this->m[this->_z0_row * _cost_matrix_size + this->_z0_col]._series = true;
  while(!isFinished)
    {
      unsigned int row_of_starred_zero = 0;
      unsigned int col_of_primed_zero = 0;
      bool is_starred_zero_in_col = false;
      bool is_primed_zero_in_row = false;

      is_starred_zero_in_col = _find_starred_zero_in_col(_z0_col, row_of_starred_zero);
      if (is_starred_zero_in_col && !m[row_of_starred_zero * _cost_matrix_size + _z0_col]._series)
	{
	  // add the starred zero to the series
	  this->m[row_of_starred_zero * _cost_matrix_size + _z0_col]._series = true;

	  // update row
	  this->_z0_row = row_of_starred_zero;

	  // add the primed zero to the series
	  is_primed_zero_in_row = _find_primed_zero_in_row(row_of_starred_zero, col_of_primed_zero);
	  this->m[row_of_starred_zero * _cost_matrix_size + col_of_primed_zero]._series = true;

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
  unsigned int nb_elem = 0;
  for (unsigned int row = 0; row < this->_cost_matrix_size; ++row)
    {
      for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
	{
	  if (this->m[row * _cost_matrix_size + col]._series)
	    {
	      if (this->m[row * _cost_matrix_size + col]._starred)
		{
		  this->m[row * _cost_matrix_size + col]._starred = false;
		  if (_starred_col[col]) --_starred_col[col]; // __starred_col__cached
		}
	      if (this->m[row * _cost_matrix_size + col]._primed)
		{
		  this->m[row * _cost_matrix_size + col]._primed = false;
		  this->m[row * _cost_matrix_size + col]._starred = true;
		  ++_starred_col[col]; // __starred_col__cached
		}
	      this->m[row * _cost_matrix_size + col]._series = false;
	      ++nb_elem;
	    }
	  if (this->m[row * _cost_matrix_size + col]._primed)
	    this->m[row * _cost_matrix_size + col]._primed = false;	

	}
      // We can also to the reset of all covered row and columns to avoid
      // useless looping
      _state_row[row] = false;
      _state_col[row] = false; // __uncovered__
      _last_visited = 0;
    }

  // Go to step1
  step = 1;
}

void BinSlay::Hungarian::_step4(int &step)
{
  // std::cout << "BEFORE STEP4:" << std::endl;
  // this->display();
  // std::cout << std::endl;
  //sleep(1);

  // Get the smallest uncovered element
  unsigned int min_uncovered_element = _get_min_uncovered_elem();

  // Add 'min_uncovered_element' to every element in covered rows
  for (unsigned int row = 0; row < _cost_matrix_size; ++row)
    {
      if (_state_row[row])
	for (unsigned int col = 0; col < _cost_matrix_size; ++col)
	  {
	    if (!this->m[row * _cost_matrix_size + col]._cost)
	      {
		this->m[row * _cost_matrix_size + col]._primed = false;
		if (this->m[row * _cost_matrix_size + col]._starred)
		  {
		    this->m[row * _cost_matrix_size + col]._starred = false;
		    if (_starred_col[col]) --_starred_col[col]; // __starred_col__cached
		  }
	      }
	    this->m[row * _cost_matrix_size + col]._cost += min_uncovered_element;
	  }
    }

  // Substract 'min_uncovered_element' from every element in uncovered columns
  for (unsigned int col = 0; col < _cost_matrix_size; ++col)
    {
      if (!_state_col[col])
	for (unsigned int row = 0; row < _cost_matrix_size; ++row)
	  {
	    this->m[row * _cost_matrix_size + col]._cost -= min_uncovered_element;
	  }
    }

  // Go to step2
  step = 2;
}

bool BinSlay::Hungarian::_TEST(unsigned int &r, unsigned int &c)
{
  std::cout << "test!" << std::endl;
  r = 0;
  c = 0;
  return true;
}

bool BinSlay::Hungarian::_find_uncovered_zero(unsigned int &r, unsigned int &c)
{
  unsigned int nb_pass = 0;

 redo_check:

  if (_search_uncovered_zeros_by_col)
    {
      for (unsigned int col = _last_visited; col < _cost_matrix_size; ++col)
	{
	  if (_state_col[col])
	    continue;
	  for (unsigned int row = 0; row < _cost_matrix_size; ++row)
	    {
	      if (!_state_row[row] && !m[row * _cost_matrix_size + col]._cost)
		{
		  r = row;
		  c = col;
		  _last_visited = col;
		  return true;
		}
	    }
	}
    }
  else
    {
      for (unsigned int row = _last_visited; row < _cost_matrix_size; ++row)
  	{
  	  if (_state_row[row])
  	    continue;
  	  for (unsigned int col = 0; col < _cost_matrix_size; ++col)
  	    {
  	      if (!_state_col[col] && !m[row * _cost_matrix_size + col]._cost)
  		{
  		  r = row;
  		  c = col;
  		  _last_visited = row;
  		  return true;
  		}
  	    }
  	}
    }
  ++nb_pass;
  _last_visited = 0;
  if (nb_pass == 1)
    goto redo_check;
  return false;
}

bool BinSlay::Hungarian::_is_starred_zero_in_row(
		 unsigned int row, unsigned int &ret) const
{
  unsigned int is_starred_zero_in_row = false;

  for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
    {
      if (!_starred_col[col])
      	continue;

      if (!this->m[row * _cost_matrix_size + col]._cost && // _cost = 0
	  this->m[row * _cost_matrix_size + col]._starred) // _starred = true
	{
	  is_starred_zero_in_row = true;
	  ret = col;
	  break;
	}
    }
  return is_starred_zero_in_row;
}

bool BinSlay::Hungarian::_find_starred_zero_in_col(unsigned int col, unsigned int &r) const
{
  bool is_starred_zero_is_in_row = false;

  for (unsigned int row = 0; row < this->_cost_matrix_size; ++row)
    {
      if (!_starred_col[col])
      	continue;

      if (!this->m[row * _cost_matrix_size + col]._cost && // _cost = 0
	  this->m[row * _cost_matrix_size + col]._starred) // _starred = true
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

  for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
    {
      if (!this->m[row * _cost_matrix_size + col]._cost && // _cost = 0
	  this->m[row * _cost_matrix_size + col]._primed) // _primed = true
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

  for (unsigned int row = 0; row < this->_cost_matrix_size; ++row)
    {
      if (_state_row[row])
	continue;
      for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
	if (!_state_col[col] && this->m[row * _cost_matrix_size + col]._cost < min_uncovered_element)
	  min_uncovered_element = this->m[row * _cost_matrix_size + col]._cost;
    }
  return min_uncovered_element;
}

inline bool BinSlay::Hungarian::_is_all_columns_covered() const
{
  bool all_covered = true;

  for (unsigned int col = 0; col < this->_cost_matrix_size; ++col)
    if (!_state_col[col])
      {
	all_covered = false;
	break;
      }
  return all_covered;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////

// void BinSlay::Hungarian::display_upper_left_submatrix() const
// {
//   std::cout << this->_cost_matrix.size() << std::endl;
//   for (unsigned int i = 0; i < this->_osize_g1; i++)
//     {
//       for (unsigned int j = 0; j < this->_osize_g2; j++)
// 	{
// 	  std::cout << this->_cost_matrix[i][j]._cost << " ";
// 	}
//       std::cout << std::endl;
//     }
// }

// void BinSlay::Hungarian::display_upper_right_submatrix() const
// {
//   for (unsigned int i = 0; i < this->_osize_g1; i++)
//     {
//       for (unsigned int j = this->_osize_g2; j < this->_cost_matrix.size(); j++)
// 	{
// 	  std::cout << this->_cost_matrix[i][j]._cost << " ";
// 	}
//       std::cout << std::endl;
//     }
// }

// void BinSlay::Hungarian::display_bottom_left_submatrix() const
// {
//   for (unsigned int i = this->_osize_g1; i < this->_cost_matrix.size(); i++)
//     {
//       for (unsigned int j = 0; j < this->_osize_g2; j++)
// 	{
// 	  std::cout << this->_cost_matrix[i][j]._cost << " ";
// 	}
//       std::cout << std::endl;
//     }
// }

// void BinSlay::Hungarian::display_bottom_right_submatrix() const
// {
//   for (unsigned int i = this->_osize_g1; i < this->_cost_matrix.size(); i++)
//     {
//       for (unsigned int j = this->_osize_g2; j < this->_cost_matrix.size(); j++)
// 	{
// 	  std::cout << this->_cost_matrix[i][j]._cost << " ";
// 	}
//       std::cout << std::endl;
//     }
// }

void BinSlay::Hungarian::display() const
{
  for (unsigned int i = 0; i < _cost_matrix_size; i++)
    {
      for (unsigned int j = 0; j < _cost_matrix_size; j++)
	{
	  if (this->m[i* _cost_matrix_size +j]._cost < 4000000)
	    std::cout << std::dec << std::setw(4) << std::setfill(' ')
		      << this->m[i* _cost_matrix_size +j]._cost;
	  else
	    std::cout << "inf";	    
	  if (this->m[i* _cost_matrix_size +j]._starred)
	    std::cout << "*";
	  if (this->m[i* _cost_matrix_size +j]._primed)
	    std::cout << "'";
	  std::cout << "  ";
	}
      std::cout << std::endl;
    }
}

/**
 * \file Hungarian.hh
 * \author Martial Bourquin
 * \version 1.0
 * \date 30/06/2012
 */

#ifndef __HUNGARIAN_HH__
# define __HUNGARIAN_HH__

# include <iostream>
# include <vector>
# include <set>
# include <stack>

namespace BinSlay
{
  class Hungarian
  {
  public:
    struct Cost
    {
      // Using 4 separated matrix should speed the computation time
      unsigned int	_cost;
      unsigned int	_saved_cost;
      bool		_starred;
      bool		_primed;
    };

  public:
    Hungarian(BinSlay::Hungarian::Cost *m, unsigned int m_size, bool);
    Hungarian(Hungarian const &o);
    ~Hungarian();
    Hungarian& operator=(Hungarian const &o);

  public:
    void solve();
    void display() const;

  private:

    // void _step0(int &step);
    // void _step1(int &step);
    // void _step2(int &step);
    // void _step3(int &step);
    // void _step4(int &step);
    // bool _is_all_columns_covered() const;
    // bool _is_row_covered(unsigned int row) const;
    // bool _is_col_covered(unsigned int row) const;
    // unsigned int _get_min_uncovered_elem() const;
    // bool _find_uncovered_zero(unsigned int &r, unsigned int &c);
    // bool _find_starred_zero_in_col(unsigned int col, unsigned int &ret) const;
    // bool _find_primed_zero_in_row(unsigned int row, unsigned int &ret) const;
    // bool _is_starred_zero_in_row(unsigned int row, unsigned int &ret) const;

    inline void _step0(int &step);
    inline void _step1(int &step);
    inline void _step2(int &step);
    inline void _step3(int &step);
    inline void _step4(int &step);
    inline bool _is_all_columns_covered() const;
    inline bool _is_row_covered(unsigned int row) const;
    inline bool _is_col_covered(unsigned int row) const;
    inline unsigned int _get_min_uncovered_elem() const;
    inline bool _find_uncovered_zero(unsigned int &r, unsigned int &c);
    inline bool _find_starred_zero_in_col(unsigned int col, unsigned int &ret) const;
    inline bool _find_primed_zero_in_row(unsigned int row, unsigned int &ret) const;
    inline bool _is_starred_zero_in_row(unsigned int row, unsigned int &ret) const;


  private:
    size_t     	_cost_matrix_size;
    Cost       	*_matrix;
    size_t     	_z0_row;
    size_t     	_z0_col;
    size_t     	_last_visited;
    size_t	_recently_uncovered;
    bool       	_search_uncovered_zeros_by_col;

    bool			*_state_row;
    bool			*_state_col;
    unsigned int       		*_starred_col;
    unsigned int       		*_primed_col;
    bool			*_primed;
    bool			*_starred;
    std::vector<Cost>		_series;
  };
}

#endif // !__HUNGARIAN_HH__

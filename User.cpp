

#include "User.h"
#include "RecommendationSystem.h"
#include <algorithm>
#include <vector>

// implement your cpp code here
User::User (const std::string &user_name, rank_map &user_ranking,
     User::recommendation_system_ptr& rec_system) : _user_name (user_name),
      _user_ranking (user_ranking), _recommendation_system_ptr (rec_system)
{}


void User::add_movie_to_rs (
  const std::string &name, int year, const std::vector<double>&features,
  double rate)
{
  sp_movie movie = _recommendation_system_ptr->add_movie (name,year,features);
  _user_ranking[movie] = rate;
}

sp_movie User::get_recommendation_by_content () const
{
  return _recommendation_system_ptr->recommend_by_content (*this);
}

double User::get_prediction_score_for_movie
(const std::string &name, int year, int k) const
{

  sp_movie movie=_recommendation_system_ptr->get_movie (name,year);
  return _recommendation_system_ptr->predict_movie_score (*this,movie,k);
}
sp_movie User::get_recommendation_by_cf (int k) const
{
  return _recommendation_system_ptr->recommend_by_cf(*this,k);
}
std::ostream &operator<< (std::ostream &os, const User &user)
{
  os <<"name: "<< user.get_name() << "\n" << *user
      ._recommendation_system_ptr<<std::endl;
  return os;
}



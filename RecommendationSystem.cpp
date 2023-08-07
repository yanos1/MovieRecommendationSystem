//
// Created by Yan Nosrati on 6/21/2023.
//
#include "RecommendationSystem.h"
#include <cmath>
#include  <algorithm>

sp_movie
RecommendationSystem::add_movie (const std::string &name, int year,
                                 const std::vector<double> &features)
{
  sp_movie movie_ptr = std::make_shared<Movie> (name, year);
  movie_bst.insert(std::pair<sp_movie,std::vector<double>>
  (movie_ptr,
  features));
  return movie_ptr;
}
sp_movie RecommendationSystem::recommend_by_content (const User &user)
{
  auto averaged_ranks_vector = get_rank_vector (user, true);
  auto preference_vector = get_preference_vector (averaged_ranks_vector);
  auto unranked_movies = get_unranked_movies (user);
  auto best_movie = get_best_movie (preference_vector, unranked_movies);
  return best_movie;
}

sp_movie RecommendationSystem::get_best_movie (std::vector<double> &
preference_vector, std::vector<sp_movie> &unranked_movies)
{
  std::pair<sp_movie, double> best_movie (nullptr, 0);
  for (const auto &movie_ptr: unranked_movies)
  {
    double angle = get_angle (preference_vector, movie_bst[movie_ptr]);
    if (!best_movie.first || angle > best_movie.second)
    {
      best_movie = std::make_pair (movie_ptr, angle);
    }
  }
  return best_movie.first;
}

double RecommendationSystem::get_movie_average (const User &user) const
{
  double sum = 0;
  int count = 0;
  auto user_ranks = user.get_ranks ();
  for (const auto &pair: user_ranks)
  {
      sum += pair.second;
      count++;
  }

  return double(sum / count);
}

std::vector<std::pair<sp_movie, double>>
RecommendationSystem::get_rank_vector (const User &user, bool
take_average)
const
{
  std::vector<std::pair<sp_movie, double>> sol_vector;
  double average = get_movie_average (user);
  for (const auto &pair: user.get_ranks ())
  {
      sp_movie cur_movie = pair.first;
      double rank = take_average ? pair.second - average : pair.second;
      sol_vector.emplace_back (cur_movie, rank);
    }

  return sol_vector;
}

std::vector<double>
RecommendationSystem::scalar_multiplication (double c,
                                             const std::vector<double>
                                             &vector)
{
  auto vec_cpy = vector;
  for (auto &element: vec_cpy)
  {
    element *= c;
  }
  return vec_cpy;
}
int RecommendationSystem::get_features_size ()
{
  return (int) movie_bst.begin ()->second.size ();
}

std::vector<double>
RecommendationSystem::get_preference_vector (std::vector<std::pair<sp_movie,
    double>> &rank_vector)
{
  auto size_of_features = get_features_size ();
  std::vector<double> preference_vector (size_of_features, 0);

  for (const auto &pair: rank_vector)
  {
    std::vector<double> cur_features_mult_by_rank = scalar_multiplication
        (pair.second, movie_bst[pair.first]);
    for (int i = 0; i < size_of_features; i++)
    {
      preference_vector[i] += cur_features_mult_by_rank[i];
    }
  }
  return preference_vector;
}
std::vector<sp_movie> RecommendationSystem::get_unranked_movies (
    const User &user)
{
  std::vector<sp_movie> sol;
  auto user_ranks = user.get_ranks ();

  for (auto &movie: movie_bst)
  {
    if (user_ranks.count (movie.first)
        == 0)//find movies that user haven't seen
      // yet
    {
      sol.emplace_back (movie.first);
    }
  }
  return sol;
}
double RecommendationSystem::dot_product (const std::vector<double> &v1, const
std::vector<double> &v2)
{
//  double result = 0.0;
//  for (size_t i = 0; i < v1.size (); i++)
//  {
//    result += v1[i] * v2[i];
//  }
  double result=0;
  for(std::vector<double>::size_type i=0;i<v1.size();i++){
    result+=v1[i]*v2[i];
  }
  return result;
}
double
RecommendationSystem::get_angle (std::vector<double> &vector_1,
                                 std::vector<double> &vector_2)
{
//  return dot_product (vector_1, vector_2) / (sqrt (dot_product
//  (vector_1, vector_1))* sqrt (dot_product (vector_2, vector_2)));
  double up=dot_product(vector_1,vector_2);
  double down=(std::sqrt (std::abs(dot_product(vector_1,vector_1))))*(std::sqrt
      (std::abs(dot_product(vector_2,vector_2))));
  return (up/down);
}

sp_movie RecommendationSystem::recommend_by_cf (const User &user, int k)
{
  auto unranked_movies = get_unranked_movies (user);
  std::pair<sp_movie, double> best_movie (nullptr, 0);
  for (const auto &movie_ptr: unranked_movies)
  {
    double cur_score = predict_movie_score (user, movie_ptr, k);
    if (!best_movie.first || cur_score > best_movie.second)
    {
      best_movie = std::make_pair (movie_ptr, cur_score);
    }
  }
  return best_movie.first;
}
double
RecommendationSystem::predict_movie_score
    (const User &user, const sp_movie &movie, int k)
{
  auto seen_movies = get_rank_vector (user);
  auto k_most_similar_movies = get_k_most_similar
      (seen_movies, movie, k);
  auto movie_prediction =
      get_movie_prediction (k_most_similar_movies, user);
  return movie_prediction;
}

bool compare_pairs (const std::pair<sp_movie, double> &p1, const
std::pair<sp_movie, double> &p2)
{
  return p1.second > p2.second;  // Compare based on the second element
  // (int) in ascending order
}

std::vector<std::pair<sp_movie, double>> RecommendationSystem::
get_k_most_similar (std::vector<std::pair<sp_movie,
    double>> &seen_movies, const sp_movie &shared_ptr, int k)
{
  std::vector<std::pair<sp_movie, double>> sol_vector;
  for (const auto &movie: seen_movies)
  {


    double angle = get_angle (movie_bst[shared_ptr], movie_bst[movie.first]);
    sol_vector.emplace_back (movie.first, angle);
  }
  std::sort (sol_vector.begin (), sol_vector.end (), compare_pairs);
  for (long unsigned int i = sol_vector.size () - 1;
       i > sol_vector.size () - k -1 ; i--)
  {
    sol_vector.pop_back ();
  }

  return sol_vector;
}
double
RecommendationSystem::get_movie_prediction (std::vector<std::pair<sp_movie,
    double>> &movies_similarities, const User &user)
{
  rank_map user_ranks = user.get_ranks ();
  double sum1 = 0;
  double sum2 = 0;
  for (const auto &pair: movies_similarities)
  {
    sum1 += pair.second * user_ranks[pair.first];
    sum2 += pair.second;
  }
  return sum1 / sum2;

}

bool comperator::operator() (const sp_movie &lhs, const sp_movie &rhs) const
{
  return (*lhs) < (*rhs);


}

sp_movie
RecommendationSystem::get_movie (const std::string &name, int year) const
{
  sp_movie res = nullptr;
  std::shared_ptr<Movie> needed_movie = std::make_shared<Movie> (name, year);
  if (movie_bst.find (needed_movie) != movie_bst.end ())
  {
    res = movie_bst.find (needed_movie)->first;

  }

  return res;

}
std::ostream &operator<< (std::ostream &os, RecommendationSystem
&recommendation_system)
{
  for (const auto &movie: recommendation_system.movie_bst)
  {
    os << movie.first->get_name () << "\n";
  }
  return os;
}







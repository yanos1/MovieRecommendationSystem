//
// Created on 2/20/2022.
//

#ifndef RECOMMENDATIONSYSTEM_H
#define RECOMMENDATIONSYSTEM_H
#include "User.h"
#include <map>
#include <vector>


struct comperator
{
    bool operator()(const sp_movie &lhs, const sp_movie &rhs) const;
};

class RecommendationSystem
{
 private:
  std::map<sp_movie,std::vector<double>,comperator> movie_bst;

  double get_movie_average (const User &user) const;
  std::vector<std::pair<sp_movie , double>> get_rank_vector (const
  User &user,bool take_average = false) const;

  static std::vector<double> scalar_multiplication
  (double c, const std::vector<double> &vector);

  int get_features_size ();

  std::vector<sp_movie> get_unranked_movies (const User &user);

  sp_movie get_best_movie(std::vector<double>& preference_vector,
                          std::vector<sp_movie>&unranked_movies);
  double dot_product(const std::vector<double>& v1, const
  std::vector<double>&);
  std::vector<double>
  get_preference_vector (std::vector<std::pair<sp_movie , double>>
  &vector_1);

  double get_angle (std::vector<double> &vector_1, std::vector<double>&
      vector_2);
  std::vector<std::pair<sp_movie , double>>
  get_k_most_similar (std::vector<std::pair<sp_movie ,double>>&
  seen_movies, const sp_movie &shared_ptr, int k);
  double
  get_movie_prediction (std::vector<std::pair<sp_movie, double>>& vector_1,
                        const User& user);

 public:


   explicit RecommendationSystem(){};
  /**
   * adds a new movie to the system
   * @param name name of movie
   * @param year year it was made
   * @param features features for movie
   * @return shared pointer for movie in system
   */
  sp_movie
  add_movie (const std::string &name,
                                int year, const std::vector<double> &features);

  /**
   * a function that calculates the movie with highest score based on
   * movie features
   * @param ranks user ranking to use for algorithm
   * @return shared pointer to movie in system
   */
  sp_movie recommend_by_content (const User &user);

  /**
   * a function that calculates the movie with highest predicted
   * score based on ranking of other movies
   * @param ranks user ranking to use for algorithm
   * @param k
   * @return shared pointer to movie in system
   */
  sp_movie recommend_by_cf (const User &user, int k);

  /**
   * Predict a user rating for a movie given argument using item cf
   * procedure with k most similar movies.
   * @param user_rankings: ranking to use
   * @param movie: movie to predict
   * @param k:
   * @return score based on algorithm as described in pdf
   */
  double predict_movie_score (const User &user, const sp_movie &movie,
                              int k);

  /**
   * gets a shared pointer to movie in system
   * @param name name of movie
   * @param year year movie was made
   * @return shared pointer to movie in system
   */
  sp_movie get_movie (const std::string &name, int year) const;


  friend std::ostream& operator<<(std::ostream& os,RecommendationSystem&
  recommendation_system);


};

#endif //RECOMMENDATIONSYSTEM_H

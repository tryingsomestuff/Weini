#include "ExtendedPosition.h"

#include "CLI.h"
#include "thread.h"

#include <algorithm>
#include <cctype>
#include <iterator>
#include <numeric>
#include <locale>
#include <iostream>
#include <fstream>
#include <iomanip>

namespace{
void Split( std::vector<std::string> & v, const std::string & str, const std::string & sep){
    size_t start = 0, end = 0;
    while ( end != std::string::npos){
        end = str.find( sep, start);
        v.push_back( str.substr( start,
                       (end == std::string::npos) ? std::string::npos : end - start));
        start = (   ( end > (std::string::npos - sep.size()) )
                  ?  std::string::npos  :  end + sep.size());
    }
}

// trim from start
std::string &ltrim(std::string &s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(),
            std::not1(std::ptr_fun<int, int>(std::isspace))));
    return s;
}
}

ExtendedPosition::ExtendedPosition(const std::string & s, bool withMoveCount) : Position(s){

    std::vector<std::string> strList;
    std::stringstream iss(s);
    std::copy(std::istream_iterator<std::string>(iss),
              std::istream_iterator<std::string>(),
              back_inserter(strList));

    if ( strList.size() < (withMoveCount?7u:5u) ){
        LOG(logFATAL) << "Not an extended position";
    }

    // remove first 6 elements (corresponding to standard position)
    std::vector<std::string>(strList.begin()+(withMoveCount?6:4), strList.end()).swap(strList);

    std::string extendedParamsStr = std::accumulate(strList.begin(), strList.end(), std::string(""),
                                                    [](const std::string & a, const std::string & b) {
                                                       return a + ' ' + b;
                                                    });
    //LOG(logINFO) << "extended parameters : " << extendedParamsStr;

    std::vector<std::string> strParamList;
    Split(strParamList,extendedParamsStr,";");
    for(size_t k = 0 ; k < strParamList.size() ; ++k){
        //LOG(logINFO) << "extended parameters : " << k << " " << strParamList[k];
        strParamList[k] = ltrim(strParamList[k]);
        if ( strParamList[k].empty()) continue;
        std::vector<std::string> pair;
        Split(pair,strParamList[k]," ");
        if ( pair.size() < 2 ){
            LOG(logFATAL) << "Not un extended parameter pair";
        }
        std::vector<std::string> values = pair;
        values.erase(values.begin());
        _extendedParams[pair[0]] = values;
        //LOG(logINFO) << "extended parameters pair : " << pair[0] << " => " << values[0];
    }
}

bool ExtendedPosition::ShallFindBest(){
    return _extendedParams.find("bm") != _extendedParams.end();
}

std::vector<std::string> ExtendedPosition::BestMoves(){
    return _extendedParams["bm"];
}

std::vector<std::string> ExtendedPosition::BadMoves(){
    return _extendedParams["am"];
}

std::string ExtendedPosition::Id(){
    if ( _extendedParams.find("id") != _extendedParams.end() ){
       return _extendedParams["id"][0];
    }
    else return "";
}

bool ExtendedPosition::ReadEPDFile(const std::string & fileName, std::vector<std::string> & positions){
    LOG(logINFO) << "Loading EPD file : " << fileName;
    std::ifstream str(fileName);
    if (str) {
        std::string line;
        while (std::getline(str, line)) {
           positions.push_back(line);
        }
        return true;
    }
    else {
        LOG(logERROR) << "Cannot open EPD file " << fileName;
        return false;
    }
}

void ExtendedPosition::Test(const std::vector<std::string> & positions,
                            const std::vector<int> &         timeControls,
                            bool                             breakAtFirstSuccess,
                            const std::vector<int> &         scores,
                            std::function< int(int) >        eloF,
                            bool                             withMoveCount,
                            bool                             display){
    struct Results{
       Results():k(0),t(0),score(0){}
       int k;
       int t;
       std::string name;
       std::vector<std::string> bm;
       std::vector<std::string> am;
       std::string computerMove;
       int score;
    };

    if (scores.size() != timeControls.size()){
        LOG(logFATAL) << "Wrong timeControl versus score vector size";
    }

    Results ** results = new Results*[positions.size()];

    // run the test and fill results table
    for (size_t k = 0 ; k < positions.size() ; ++k ){
       //LOG(logINFO) << "Test #" << k << " " << positions[k];
       results[k] = new Results[timeControls.size()];
       ExtendedPosition extP(positions[k],withMoveCount);
       for(size_t t = 0 ; t < timeControls.size() ; ++t){
          //LOG(logINFO) << " " << t;
          Definitions::timeControl.currentMoveMs = timeControls[t];
          SearcherBase::SearchedMove ret = TestAnalysis(extP.GetFEN(),MAX_SEARCH_DEPTH, Searcher::eVerb_standard);

          results[k][t].name = extP.Id();
          results[k][t].k = (int)k;
          results[k][t].t = (int)Definitions::timeControl.currentMoveMs;

          Move & m = SearcherBase::GetMove(ret);
          m.Validate(extP,true,true);
          results[k][t].computerMove = m.ShowAlgAbr(extP);
          LOG(logINFO) << "Best move found is  " << results[k][t].computerMove;

          if ( extP.ShallFindBest()){
             LOG(logINFO) << "Best move should be " << extP.BestMoves();
             results[k][t].bm = extP.BestMoves();
             results[k][t].score = 0;
             bool success = false;
             for(size_t i = 0 ; i < results[k][t].bm.size() ; ++i){
                if ( results[k][t].computerMove == results[k][t].bm[i]){
                   results[k][t].score = scores[t];
                   success = true;
                   break;
                }
             }
             if ( breakAtFirstSuccess && success ) break;
          }
          else{
             LOG(logINFO) << "Bad move was " << extP.BadMoves();
             results[k][t].am = extP.BadMoves();
             results[k][t].score = scores[t];
             bool success = true;
             for(size_t i = 0 ; i < results[k][t].am.size() ; ++i){
                if ( results[k][t].computerMove == results[k][t].am[i]){
                   results[k][t].score = 0;
                   success = false;
                   break;
                }
             }
             if ( breakAtFirstSuccess && success ) break;
          }
       }
    }

    // display results
    int totalScore = 0;
    std::cout << std::setw(25) << "Test"
              << std::setw(14) << "Move";
    for(size_t j = 0 ; j < timeControls.size() ; ++j){
       std::cout << std::setw(8) << timeControls[j];
    }
    std::cout << std::setw(6) << "score" << std::endl;
    for (size_t k = 0 ; k < positions.size() ; ++k ){
        int score = 0;
        for(size_t t = 0 ; t < timeControls.size() ; ++t){
           score += results[k][t].score;
        }
        totalScore += score;
        std::stringstream str;
        str << (results[k][0].bm.empty()?results[k][0].am:results[k][0].bm);
        std::cout << std::setw(25) << results[k][0].name
                  << std::setw(14) << (results[k][0].bm.empty()?std::string("!")+str.str():str.str());
        for(size_t j = 0 ; j < timeControls.size() ; ++j){
            std::cout << std::setw(8) << results[k][j].computerMove;
        }
        std::cout << std::setw(6) << score << std::endl;
    }

    if ( eloF(100) != 0) {
       LOG(logINFO) << "Total score " << totalScore << " => ELO " << eloF(totalScore);
    }

    // clear results table
    for (size_t k = 0 ; k < positions.size() ; ++k ){
       delete[] results[k];
    }
    delete[] results;
}

void ExtendedPosition::TestStatic(const std::vector<std::string> & positions,
	                              int                              chunck,
	                              bool                             withMoveCount,
                                  bool                             display) {
	struct Results {
		Results() :k(0), t(0), score(0){}
		int k;
		int t;
        ScoreType score;
		std::string name;
	};

	Results * results = new Results[positions.size()];

	// run the test and fill results table
	for (size_t k = 0; k < positions.size(); ++k) {
		std::cout << "Test #" << k << " " << positions[k] << std::endl;
		ExtendedPosition extP(positions[k], withMoveCount);
		//std::cout << " " << t << std::endl;
        ScoreType ret = TestStaticAnalysis(extP.GetFEN());

		results[k].name = extP.Id();
		results[k].k = (int)k;
		results[k].score = ret;

		std::cout << "score is  " << ret << std::endl;

	}

	// display results
    std::cout << std::setw(25) << "Test" << std::setw(14) << "score" << std::endl;
    ScoreType score = 0;
	for (size_t k = 0; k < positions.size(); ++k) {
		std::cout << std::setw(25) << results[k].name << std::setw(14) << results[k].score << std::endl;
        if ( k%chunck == 0 ){
           score = results[k].score;
        }
        // only compare unsigned score ...
        if ( std::abs( std::abs(results[k].score) - std::abs(score) ) > 0 ){
            LOG(logWARNING) << "Score differ !";
        }
	}

	// clear results table
	delete[] results;
}


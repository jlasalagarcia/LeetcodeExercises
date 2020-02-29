class Solution {
public:
   vector<vector<int>> combinationSum(vector<int>& candidates, int target) {
      constexpr int minReserve = 5;
      vector<vector<int>> solutions;
      solutions.reserve(minReserve);

      // Sort candidates once
      sort(candidates.begin(), candidates.end());

      sortedCombinationSum(solutions, &candidates[0], candidates.size(), target, 0);

      return solutions;
   }

   void sortedCombinationSum(vector<vector<int>> &solutions, const int *pCandidates, int sizeCandidates, int target, int parentSize)
   {
      // Last candidate out of "for"
      for(int i = sizeCandidates - 1; i > 0; --i)
      {
         const auto num = *(pCandidates + i);
         if(num == target)
         {
            solutions.emplace_back(1+parentSize, num);
         }
         else if(num < target)
         {
            auto subTarget = target - num;
            if(subTarget == *pCandidates)
            {
               solutions.emplace_back(2+ parentSize);
               auto &solution = solutions.back();
               solution[0] = *pCandidates;
               solution[1] = num;
            }
            else if(subTarget > *pCandidates)
            {
               const int maxRepeat = target / num;
               for(int repeat = maxRepeat; repeat > 0; --repeat)
               {
                  const int repeatNum = repeat * num;
                  if(repeatNum == target)
                  {
                     solutions.emplace_back(repeat + parentSize, num); 
                  }
                  else
                  {
                     subTarget = target - repeatNum;
                     if(subTarget == *pCandidates)
                     {
                        solutions.emplace_back(1+repeat + parentSize, num);
                        solutions.back()[0] = *pCandidates;
                     }
                     //
                     // unique recursive call
                     //
                     else if(subTarget > *pCandidates)
                     {
                        const int preSize = solutions.size();
                        sortedCombinationSum(solutions, pCandidates, i, subTarget, parentSize+repeat);
                        const int postSize = solutions.size();

                        for(int j = preSize; j < postSize; ++j)
                        {
                           auto &solution = solutions[j];
                           const int end = solution.size() - parentSize;
                           for(int k = end - repeat; k < end; k++)
                           {
                              solution[k] = num;
                           }   
                        }
                     }
                  }
               }
            }
         }
      } // for i

      // Last candidate
      {
         const auto num = *pCandidates;
         const int maxRepeat = target / num;
         if(maxRepeat * num == target)
         {
            solutions.emplace_back(maxRepeat + parentSize, num);
         }
      }
   }
};

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

#include "ortools/base/logging.h"
#include "ortools/sat/cp_model.h"
#include "ortools/sat/cp_model.pb.h"
#include "ortools/sat/cp_model_solver.h"



namespace operations_research
{
    namespace sat
    {
        int SearchReg(std::vector<int> RegCoveredBySet, int numReg)
        {
            for (int nr : RegCoveredBySet)
            {
                if (nr == numReg)
                {
                    return 1;
                }
            }

            return 0;
        }

        void cpSolver()
        {
            CpModelBuilder cp_model;

            std::ifstream myfile;
            myfile.open("../data/sc_27_0", std::ios::in);
            if (myfile.is_open())
            {
                int NumberOfReg, NumberOfSetsCoveringReg;
                std::vector<std::vector<int>> RegCoveredBySets;
                std::vector<int> RegCoveredBySet;
                std::vector<int> CostsOfSets;
                std::string CostReg;
                int Reg, cost;
                int i, j;

                std::getline(myfile, CostReg);
                std::stringstream stream(CostReg);
                stream >> NumberOfReg >> NumberOfSetsCoveringReg;
                for (i = 0; i < NumberOfSetsCoveringReg; ++i)
                {
                    std::getline(myfile, CostReg);
                    std::stringstream stream(CostReg);
                    stream >> cost;
                    CostsOfSets.push_back(cost);
                    while (stream >> Reg)
                    {
                        RegCoveredBySet.push_back(Reg);
                    }
                    RegCoveredBySets.push_back(RegCoveredBySet);
                    RegCoveredBySet.clear();
                }


                std::vector<BoolVar> SetsTake(NumberOfSetsCoveringReg);

                for (i = 0; i < NumberOfSetsCoveringReg; ++i)
                {
                    SetsTake[i] = cp_model.NewBoolVar();
                }

                std::vector<LinearExpr> RegionsCovered(NumberOfReg);
                for (j = 0; j < NumberOfReg; ++j)
                {
                    for (i = 0; i < NumberOfSetsCoveringReg; ++i)
                    {
                        RegionsCovered[j] += (SetsTake[i] * SearchReg(RegCoveredBySets[i], j));
                    }
                    cp_model.AddLessOrEqual(1, RegionsCovered[j]);
                }

                LinearExpr total_cost{ 0 };
                for (i = 0; i < NumberOfSetsCoveringReg; ++i)
                {
                    total_cost += SetsTake[i] * CostsOfSets[i];
                }
                cp_model.Minimize(total_cost);

                // Solving part.
                Model model;

                // Sets a time limit of 10 seconds.
                SatParameters parameters;
                parameters.set_max_time_in_seconds(5.0);
                model.Add(NewSatParameters(parameters));

                // Solve.
                const CpSolverResponse response = SolveCpModel(cp_model.Build(), &model);

               

                if (response.status() == CpSolverStatus::INFEASIBLE)
                {
                    std::cout << "No solution found." << std::endl;
                }

                std::cout << "Total cost: " << response.objective_value() << std::endl;

                for (i = 0; i < NumberOfSetsCoveringReg; ++i)
                {
                    if (SolutionBooleanValue(response, SetsTake[i]))
                    {
                        std::cout << 1 << " ";
                    }
                    else
                    {
                        std::cout << 0 << " ";
                    }
                }
                std::cout << std::endl;
            }
            else
            {
                std::cout << "Smth wrong with the path to the folder or smth else." << std::endl;
            }
        }
    }
}


int main(void)
{
    operations_research::sat::cpSolver();

    return 0;
}
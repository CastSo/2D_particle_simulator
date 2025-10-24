#pragma once
#include "config.h"
#include "./components/components.h"


class Think {
    public:
        Think(World& world, std::vector<std::vector<int>>& MAP, std::unordered_map<std::string, Particle>& particles);
        ~Think();

        void update_map();
    private:
        std::unordered_map<std::string, Particle>& particles;
        std::vector<std::vector<int>>& MAP;
        World& world;
        int itr_i;

        bool in_quad(int material, int upLeft, int upRight, int lowLeft, int lowRight);
        std::vector<int> process_rules(int material, bool isDestroyer, std::vector<int> state, std::vector<std::vector<int>> rules, std::vector<std::vector<int>> transitions, std::vector<std::vector<int>> transitions_p);
        std::vector<int> process_rules(int material, bool isDestroyer, std::vector<int> state, std::vector<std::vector<int>> rules, std::vector<std::vector<int>> transitions);
        void map_think(int xshift, int yshift);

};
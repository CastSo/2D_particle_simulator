#include "./think.h"


Think::Think(World& world, std::vector<std::vector<int>>& MAP, std::unordered_map<std::string, Particle>& particles):
    world(world),
    particles(particles),
    MAP(MAP){
    
    itr_i = 0;
}

Think::~Think() {

}

void Think::update_map() {
    int itrThink[4][2] = {{0,0}, {1,1}, {0,1}, {1,0}};
    
    map_think(itrThink[itr_i][0], itrThink[itr_i][1]);

    if (itr_i < 3){    
            itr_i += 1;
        }else{ 
            itr_i = 0;
        }
}


void Think::map_think(int xshift, int yshift) {
    int EMPTY = 0;
    std::vector<int> rules;
    for (int y=(world.YBOUND+yshift); y < (int)((world.SCR_HEIGHT/world.POINT_SIZE)-world.YBOUND)+yshift; y+=2)
    {
       for (int x=(world.XBOUND+xshift); x < (int)((world.SCR_WIDTH/world.POINT_SIZE)-world.XBOUND)+xshift; x+=2)
       {
            int upLeft = MAP[y+1][x];
            int upRight = MAP[y+1][x+1];
            int lowLeft = MAP[y][x];
            int lowRight = MAP[y][x+1];

            int state[4]; 

            if((upLeft == EMPTY && upRight == EMPTY && lowLeft == EMPTY && lowRight == EMPTY) ||
            (upLeft > EMPTY && upRight > EMPTY && lowLeft > EMPTY && lowRight > EMPTY) )
            {
            
                continue;
            }

            if(in_quad(particles.at("WOOD").id, upLeft, upRight, lowLeft, lowRight))
            {
                   
                if(in_quad(particles.at("FIRE").id, upLeft, upRight, lowLeft, lowRight)){
                    rules = process_rules(particles.at("FIRE").id, particles.at("FIRE").isDestroyer, {upLeft, upRight, lowLeft, lowRight}, particles.at("FIRE").rules, particles.at("FIRE").transitions,
                                            particles.at("FIRE").transitions_p);
                        
                    upLeft = rules[0];
                    upRight = rules[1];
                    lowLeft = rules[2];
                    lowRight = rules[3];
                }else{
                    continue;
                }
            }

            if(in_quad(particles.at("SAND").id, upLeft, upRight, lowLeft, lowRight)){
                rules = process_rules(particles.at("SAND").id, particles.at("SAND").isDestroyer, {upLeft, upRight, lowLeft, lowRight}, particles.at("SAND").rules, particles.at("SAND").transitions);
                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

           if(in_quad(particles.at("WATER").id, upLeft, upRight, lowLeft, lowRight)){
                rules = process_rules(particles.at("WATER").id, particles.at("WATER").isDestroyer, {upLeft, upRight, lowLeft, lowRight}, particles.at("WATER").rules, particles.at("WATER").transitions);
           
                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

           if(in_quad(particles.at("GAS").id, upLeft, upRight, lowLeft, lowRight)){
                rules = process_rules(particles.at("GAS").id, particles.at("GAS").isDestroyer, {upLeft, upRight, lowLeft, lowRight}, particles.at("GAS").rules, particles.at("GAS").transitions,
                                        particles.at("GAS").transitions_p);

                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

            
           if(in_quad(particles.at("FIRE").id, upLeft, upRight, lowLeft, lowRight)){
                rules = process_rules(particles.at("FIRE").id, particles.at("FIRE").isDestroyer, {upLeft, upRight, lowLeft, lowRight}, particles.at("FIRE").rules, particles.at("FIRE").transitions,
                                        particles.at("FIRE").transitions_p);    
                upLeft = rules[0];
                upRight = rules[1];
                lowLeft = rules[2];
                lowRight = rules[3];
            }

            MAP[y+1][x] = rules[0];
            MAP[y+1][x+1] = rules[1];
            MAP[y][x] = rules[2];
            MAP[y][x+1] = rules[3];

            
         
            
        }
    }

}

std::vector<int> Think::process_rules(int material, bool isDestroyer, std::vector<int> state, std::vector<std::vector<int>> rules,
                         std::vector<std::vector<int>> transitions)
{   
    std::vector<int> rule;
    std::vector<int> transitionN;
    for(int i = 0; i < rules.size(); i++){
        rule = {(rules[i][0]*material), (rules[i][1]*material), (rules[i][2]*material), (rules[i][3]*material)};
        if(state[0] == rule[0] && state[1] == rule[1] && state[2] == rule[2] && state[3] == rule[3]){
            transitionN = {(transitions[i][0]*material), (transitions[i][1]*material), (transitions[i][2]*material), (transitions[i][3]*material)};
            return transitionN;
        }
    }
    return state;
}

std::vector<int> Think::process_rules(int material, bool isDestroyer, std::vector<int> state, std::vector<std::vector<int>> rules, 
                        std::vector<std::vector<int>> transitions, std::vector<std::vector<int>> transitions_p)
{   
    
    int p = 1+(rand() % 3);
    std::vector<int> rule;

    for(int i = 0; i < rules.size(); i++){
        std::vector<int> transitionN;
        rule = {(rules[i][0]*material), (rules[i][1]*material), (rules[i][2]*material), (rules[i][3]*material)};
        if(isDestroyer){
            //Checks particle states in a quad
            for(int j = 0; j < 4; j++)
            {
                if(rule[j] == 0 && state[j] != material){
                    //Destructable materials
                    if(state[j] == 0 || state[j] == 1)
                    {
                       if(p == 1 && i < transitions_p.size())
                        {    
                            transitionN.push_back(transitions_p[i][j]*material);
                        } else {
                            transitionN.push_back(transitions[i][j]*material);
                        }
                    }else{
                        transitionN.push_back(state[j]);
                    }
                }else if (rule[j] == material && state[j] == material){
                    if(p == 1 && i < transitions_p.size())
                    {    
                        transitionN.push_back(transitions_p[i][j]*material);
                    } else {
                        transitionN.push_back(transitions[i][j]*material);
                    }
                } else{
                        break;
                    }
            }
            if(transitionN.size() == 4) {
                return transitionN;
            }
            transitionN.clear();

        }
        else{
            if(state[0] == rule[0] && state[1] == rule[1] && state[2] == rule[2] && state[3] == rule[3]){
                if(p == 1 && i < transitions_p.size())
                {
                    transitionN = {(transitions_p[i][0]*material), (transitions_p[i][1]*material), (transitions_p[i][2]*material), (transitions_p[i][3]*material)};
                }else{
                    transitionN = {(transitions[i][0]*material), (transitions[i][1]*material), (transitions[i][2]*material), (transitions[i][3]*material)};
                }
                return transitionN;
            }}

    }
    return state;
}

bool Think::in_quad(int material, int upLeft, int upRight, int lowLeft, int lowRight)
{
    bool inQuad = false;
    if (upLeft == material || upRight == material || lowLeft == material || lowRight == material)
    {
        inQuad = true;
    }
    return inQuad;
}

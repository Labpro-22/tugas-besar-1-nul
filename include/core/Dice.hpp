#pragma once

#include <cstdlib>
#include <ctime>
~~
class Dice {
private:
    int die1;
    int die2;
    bool manualMode;

public:
    Dice();
    
    // Roll dice randomly
    void roll();
    
    // Set dice manually (for cheating/testing)
    void setManual(int d1, int d2);
    
    // Getters
    int getDie1() const;
    int getDie2() const;
    int getTotal() const;
    
    // Check if double
    bool isDouble() const;
    
    // Reset to random mode
    void reset();
};

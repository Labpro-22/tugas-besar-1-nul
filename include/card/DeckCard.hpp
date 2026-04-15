#include "card.hpp"
class CardDeck {
private:
    std::vector<Card*> deck;
    std::vector<Card*> discard;

public:
    CardDeck() = default;

    ~CardDeck() {
        for (auto card : deck) delete card;
        for (auto card : discard) delete card;
    }

    void addCard(Card * card) {
        deck.push_back(card);
    }

    Card* draw() {
        if (deck.empty()) {
            if (discard.empty()) return nullptr;
        }
        
        Card* drawnCard = deck.back();
        deck.pop_back();
        return drawnCard;
    }

    void shuffle() {
        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(deck.begin(), deck.end(), g);
    }

    void addToDiscard(Card* card) {
        discard.push_back(card);
    }

    void refillFromDiscard() {
        deck.insert(deck.end(), discard.begin(), discard.end());
        discard.clear();
        shuffle();
        std::cout << "[INFO] Deck habis. Tempat pembuangan dikocok ulang menjadi deck baru.\n";
    }

    int size() const {
        return deck.size();
    }
};
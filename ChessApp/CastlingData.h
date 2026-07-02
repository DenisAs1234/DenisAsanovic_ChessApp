#pragma once

class King;
class Rook;
class Square;

struct CastlingData {
    King* king = nullptr;
    Rook* rook = nullptr;

    Square* kingStart = nullptr;
    Square* kingDestination = nullptr;

    Square* rookStart = nullptr;
    Square* rookDestination = nullptr;
    /*
    bool isPrepared() const {
        return king && rook &&
            kingStart && kingDestination &&
            rookStart && rookDestination;
    }*/

    void clear() {
        king = nullptr;
        rook = nullptr;

        kingStart = nullptr;
        kingDestination = nullptr;

        rookStart = nullptr;
        rookDestination = nullptr;
    }
};
#pragma once

inline int getSquareIndex(int rank, int file) {
	if (rank < 1 || rank > 8 || file < 0 || file > 7)
		return -1;
	return (rank - 1) * 8 + file;
}
# include "Type.hpp"

// Typesの比較演算子
bool operator==(const Types& a, const Types& b)
{
	return a.type1 == b.type1 && a.type2 == b.type2;
}

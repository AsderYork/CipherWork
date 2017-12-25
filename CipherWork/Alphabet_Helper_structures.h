#pragma once
// Helper struct.Contains ony x and y.Allow for simple transformation of in to complete point;
struct SubPointAndLetter
{
	int x = 0, y = 0;
	std::string String;

	SubPointAndLetter() {};
	SubPointAndLetter(Eleptic::Point& Point, std::string& Str) : x(Point.x.toInt()), y(Point.y.toInt()), String(Str) {}

	template <class Archive>
	void save(Archive & ar) const
	{
		ar(cereal::make_nvp("point_x", x),
			cereal::make_nvp("point_y", y),
			cereal::make_nvp("letter", String));
	}

	template <class Archive>
	void load(Archive & ar)
	{
		ar(cereal::make_nvp("point_x", x),
			cereal::make_nvp("point_y", y),
			cereal::make_nvp("letter", String));
	}
};


struct AlphabetPair
{

	Eleptic::Point point;
	std::string String;
	AlphabetPair(std::pair<Eleptic::Point, std::string> in) : point(in.first), String(in.second) {}
	AlphabetPair() : point(0, 0, 0) {};
	AlphabetPair(SubPointAndLetter& PAndL, Eleptic::Curve& Curve) : point(Curve.GetPoint(PAndL.x, PAndL.y)), String(PAndL.String)
	{
	}

	static std::vector<SubPointAndLetter> ToSubpointVec(std::vector<AlphabetPair>& In)
	{
		std::vector<SubPointAndLetter> Out;
		for (auto& AlPair : In)
		{
			Out.emplace_back(AlPair.point, AlPair.String);
		}
		return Out;
	}

	static std::vector<AlphabetPair> FromSubPointVec(std::vector<SubPointAndLetter>& In, Eleptic::Curve& Curve)
	{
		std::vector<AlphabetPair> Out;
		for (auto& SPndL : In)
		{
			Out.emplace_back(SPndL, Curve);
		}
		return Out;
	}

	template <class Archive>
	void save(Archive & ar) const
	{
		ar(cereal::make_nvp("point_x", point.x.toUnsignedInt()),
			cereal::make_nvp("point_y", point.y.toUnsignedInt()),
			cereal::make_nvp("point_mod", point.mod.toUnsignedInt()),
			cereal::make_nvp("point_curveCoef", point.curveCoef.toInt()),
			cereal::make_nvp("letter", String));
	}

	template <class Archive>
	void load(Archive & ar)
	{
		int x, y, mod, cc;
		ar(cereal::make_nvp("point_x", x),
			cereal::make_nvp("point_y", y),
			cereal::make_nvp("point_mod", mod),
			cereal::make_nvp("point_curveCoef", cc),
			cereal::make_nvp("letter", String));
		point = Eleptic::Point(x, y, mod, cc);
	}


};

std::vector<std::string> FromPointsToString(std::vector<AlphabetPair>& Alphabet, std::vector<Eleptic::Point> &Points)
{
	std::vector<std::string> Out;
	for (auto& Point : Points)
	{
		auto Letter = std::find_if(Alphabet.begin(), Alphabet.end(), [&](AlphabetPair &pair) {return pair.point == Point; });
		if (Letter == Alphabet.end()) { throw std::exception("There are no such point in an alphabet!"); }
		Out.push_back(Letter->String);
	}
	return Out;
}

std::vector<Eleptic::Point> ProcessMessage(std::vector<AlphabetPair> &Alph, std::vector<std::string> Msg)
{
	std::vector<Eleptic::Point> Result;
	bool found = false;
	for (auto& letter : Msg)
	{
		found = false;
		for (auto Sear : Alph)
		{
			if (letter == Sear.String)
			{
				Result.push_back(Sear.point);
				found = true;
				break;
			}
		}
		if (!found)
		{
			printf("Letter %s is not reprsented!\n", letter.c_str());
		}
	}
	return Result;
}

std::vector<std::string> LoadMessage(std::string Filename = std::string("message.json"))
{
	std::wifstream stream(Filename, std::ios_base::out);

	std::vector<std::string> Result;

	std::wstring tmp;
	while (true)
	{
		std::getline(stream, tmp);
		for (int i = 0; i< tmp.size(); i++)
		{
			auto& substr = tmp.substr(i, 1);
			Result.push_back(std::string(substr.begin(), substr.end()));
		}
		if (stream.eof()) { break; }
	}
	return Result;
}

void SaveMessage(std::vector<std::string> &in, std::string Filename = std::string("message.json"))
{
	std::ofstream stream(Filename, std::ios_base::out);

	std::wstring buf;
	for (auto& substr : in)
	{
		stream << substr;
		//buf += std::wstring(substr.begin(), substr.end());
	}

	//stream << buf;
}
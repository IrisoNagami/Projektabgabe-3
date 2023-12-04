#include "std_lib_inc.h";

/*
To do:

Modulo einfügen, mit anderen Rechenzeichen ermöglichen
Welche Rechenreihenfolge ?

Wenn Fehler geschah, dass nächste Eingabe übernommen wird

x+y; muss auch noch fertig gemacht werde, dass das auch ohne Whitespace angenommen wird.
*/
bool durchZuordner;	//Für die Änderung von Zahlen
// Globale Variablen anlegen, um Magic Constants zu vermeiden
constexpr char kLet = 'l';
constexpr char kBezeichner = 'n';
const string kDeklaration = "let";
constexpr char kZahl = '8';
constexpr char kAusgabe = ';';
constexpr char kEnde = 'q';
const string kWertZuordnung = "set";
constexpr char kSet = 's';

//Konstanten
constexpr double kPi = 3.1415926535;
constexpr double ke = 2.7182818284;
vector<string> konstanten = { "e", "pi" };


class Variable							//Klasse für Variablen anlegen
{
public:
	string name;
	double wert;
};

const Variable pi { "pi", kPi };		//pi als Variable implementieren
const Variable e { "e", ke };			//e  als Variable implementieren

class SymbolTabelle
{
public:
	double getWert(string variablen_name);
	void setWert(string variablen_name, double wert);
	bool istDeklariert(string variablen_name);
	double definiereVar(string variablen_name, double wert);
private:
	vector<Variable> variablen_;		//zum speichern aller Variablen
};

SymbolTabelle symbol_tabelle;

double SymbolTabelle::getWert(string variablen_name)
{
	for (Variable x : symbol_tabelle.variablen_)
	{
		if (variablen_name == x.name)
		{
			return x.wert;
		}
	}
	error("Die Variable ist nicht deklariert(getWert)");
}

void SymbolTabelle::setWert(string variablen_name, double wert) 
{
	for (int i = 0; i < symbol_tabelle.variablen_.size(); ++i)
	{
		Variable x = symbol_tabelle.variablen_[i];
		if (variablen_name == x.name)
		{
			symbol_tabelle.variablen_[i].wert = wert;
			return;
		}
	}
	error("Die Variable ist nicht deklariert(setWert)");
}

bool SymbolTabelle::istDeklariert(string variablen_name) 
{
	for (Variable x : symbol_tabelle.variablen_)
	{
		if (variablen_name == x.name)
		{
			return true;
		}
	}
	return false;
}

double SymbolTabelle::definiereVar(string variablen_name, double wert)		//Funktion zu definieren der Variablen
{
	if (istDeklariert(variablen_name))
	{
		error("Die Variable ist bereits deklariert");		//Wenn die Variable mit dem Namen schon existiert
	}
	else
	{
		Variable x{ variablen_name, wert };			//Vorübergehende Variable deklarieren mit Namen und Wert
		symbol_tabelle.variablen_.push_back(x);					//Variable in den Vector einfügen
		return wert;
	}
}

class Token
{
public:
	char art;
	double wert;
	string bezeichner;

	Token();
	Token(char ch);
	Token(char ch, double w);
	Token(char ch, string n);
};

Token::Token()					//Konstruktoren implementieren
{
	art = 0;
}
Token::Token(char ch)			//Überladung der Funktion Token()
{
	art = ch;
}
Token::Token(char ch, double w)
{
	art = ch;
	wert = w;
}
Token::Token(char ch, string n)
{
	art = ch;
	bezeichner = n;
}

class TokenStream
{
public:
	Token get();
	void putback(Token t);
	void ignoreUntil(char c);
private:
	Token puffer;
	bool voll{ false };
};

TokenStream token_stream;

void TokenStream::putback(Token t)			//Speichern des Token
{
	if (voll)
	{
		error("putback() in einen vollen Puffer");
	}
	puffer = t;
	voll = true;
}

Token TokenStream::get()					//Token auslesen
{
	if (voll)
	{
		voll = false;
		return puffer;
	}

	char ch;
	cin >> ch;

	switch (ch)
	{
	case kAusgabe: case kEnde:
	case '(': case ')': case '{': case '}':
	case '+': case '-': case '*': case '/': case '=':
		return Token{ ch };
	case '.':
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		cin.putback(ch);
		double wert;
		cin >> wert;
		return Token{ kZahl, wert };
	default:
		if (isalpha(ch))			//ob es ein Buchstabe ist
		{
			cin.putback(ch);
			string s;
			cin >> s;
			if (s == kDeklaration)		// ob es gleich let ist für definition der variable
			{
				return Token{ kLet };
			}
			else if (s == kWertZuordnung)		//ob es gleich set ist für veränderung der Variable
			{
				return Token{ kSet };
			}
			//workt nicht falsch, man kann den wert von variablen nicht verändern
			else if (symbol_tabelle.istDeklariert(s) && !durchZuordner)
			{
				return Token{ kZahl, symbol_tabelle.getWert(s) };
			}
			return Token{ kBezeichner, s };
		}
		error("Unbekanntes Token");
	}
}

void TokenStream::ignoreUntil(char c)		//Bis zu einem bestimmten char ignorieren
{
	if (voll && puffer.art == c)
	{
		voll = false;
		return;
	}
	voll = false;

	char ch = 0;
	while (cin >> ch)
	{
		if (ch == c)
		{
			return;
		}
	}
}

void aufraeumen()					//token_stream leeren
{
	while (true)
	{
		Token t = token_stream.get();
		if (t.art == kAusgabe)
		{
			return;
		}
	}
}

double anweisung();

void berechne()				//berechnen
{
	while (cin)
	{
		try 
		{
			Token t = token_stream.get();
			while (t.art == kAusgabe)
			{
				t = token_stream.get();
			}
			if (t.art == kEnde)			//Wenn Zeichen zum beenden eingegeben wurde
			{
				return;
			}
			token_stream.putback(t);
			cout << anweisung() << '\n';
		}
		catch (runtime_error& e)
		{
			cerr << e.what() << '\n';
			token_stream.ignoreUntil(kAusgabe);
			aufraeumen();
		}
	}
}

double ausdruck();				//Der Linker geht dann zur Funktion ausdruck (wichtig!)

double primary()				// Um stärkere Bindung durch Klammern zu überprüfen
{
	Token t = token_stream.get();
	switch (t.art)
	{
	case kZahl:
		return t.wert;
	case '-':
		return -primary();		//Wenn die eingegebene Zahl negativ ist
	case '+':
		return primary();		//Wenn die eingegebene Zahl positiv ist
	case '(':					//Klammern implementiert
	{					
		double d = ausdruck();
		t = token_stream.get();
		if (t.art != ')')
		{
			error("Öffnende '(' ohne passende schließende ')'.");
		}
		return d;
	}
	case '{':					//Geschweifte Klammern implementieren
	{
		double d = ausdruck();
		t = token_stream.get();
		if (t.art != '}')
		{
			error("Öffnende '{' ohne passende schließende '}'.");
		}
		return d;
	}
	default:
		error("Primärausdruck erwartet");
		return 0;
	}
}

double term()
{
	double links = primary();
	Token t = token_stream.get();
	while (true)
	{
		switch (t.art)
		{
		case '*':							//Multiplikation
			links *= primary();
			t = token_stream.get();
			break;
		case '/':							// Division
		{
			double d = primary();
			if (d == 0.0)					//Division durch 0 verhindern
			{
				error("Division durch 0");
			}
			links /= d;
			t = token_stream.get();
			break;
		}
		default:							//Alles andere
			token_stream.putback(t);
			return links;
		}
	}
}

double ausdruck()
{
	double links = term();
	Token t = token_stream.get();
	while (true)
	{
		switch (t.art)
		{
		case '+':					//Addition
			links += term();
			t = token_stream.get();
			break;
		case '-':					//Subtraktion
			links -= term();
			t = token_stream.get();
			break;
		default:
			token_stream.putback(t);
			return links;
		}
	}
}

void isKonstante(Token t)
{
	for (string x : konstanten)
	{
		if (t.bezeichner == x)
		{
			error("Konstanten sind nicht veränderbar");
		}
	}
}

double deklaration()										
{
	Token t = token_stream.get();							//Token lesen
	if (t.art == kBezeichner)								//Token = Bezeichner
	{
		isKonstante(t);										//Ob es eine konstante ist
		Token t2 = token_stream.get();						//weiteres Token
		if (t2.art == '=')
		{
			double x = ausdruck();							//Wert über Ausdruck lesen
			symbol_tabelle.definiereVar(t.bezeichner, x);	//Variable definieren
			return x;										//Wert der Variable zurückgeben
		}
		else
		{
			error("'=' fehlt in Deklaration");
		}
	}
	else
	{
		error("Diese Variable ist undefiniert");			//Fehlermeldung muss geändert werden, passt nicht mehr
	}

}

double zuordnung()									//Wert der Variable ändern
{
	durchZuordner = true;							// auf true, damit beim get()-Aufruf das richtige Token zurückkommt
	Token t = token_stream.get();
	if (t.art == kBezeichner)
	{
		Token t2 = token_stream.get();
		if (t2.art != '=')
		{
			error("'=' fehlt in der Neudefinition");
		}
		else
		{
			isKonstante(t);								//Ob es eine Konstante ist, wenn ja -> Exception
			double x = ausdruck();
			symbol_tabelle.setWert(t.bezeichner, x);
			durchZuordner = false;						//Auf den Standartwert setzen
			return x;
		}
	}
	else
	{
		error("zuordnung()");
	}
}

double anweisung()
{
	//lies ein Token
	Token t = token_stream.get();
	// Wenn das Token ein "let" repräsentiert
	// rufe Funktion deklaration() auf
	if (t.art == kLet)
	{
		return deklaration();
	}
	else if (t.art == kSet)
	{
		return zuordnung();
	}
	else
	{
		token_stream.putback(t);			//Token zurücklegen
		double x = ausdruck();				// Wert des Audrucks
		return x;
	}
}


int main()
{
	//Konstanten als Variable 
	symbol_tabelle.definiereVar("e", ke);
	symbol_tabelle.definiereVar("pi", kPi);
	try
	{
		berechne();
		return 0;
	}
	catch (runtime_error& e)
	{
		cerr << e.what() << '\n';
		return 1;
	}
	catch (...)
	{
		cerr << "unbekannter Fehler" << '\n';
		return 2;
	}
}

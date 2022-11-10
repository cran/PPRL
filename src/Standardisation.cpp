//============================================================================
// Name        : Bloomencoder.cpp
// Author      : Rukasz
// Version     :
// Copyright   : Your copyright notice
// Description : Ansi-style
//============================================================================

#include "Standardisation.h"

void ReplaceAllSubstr( string& source, const string& from, const string& to )
{
  string newString;
  newString.reserve( source.length() );  // avoids a few memory allocations

  string::size_type lastPos = 0;
  string::size_type findPos;

  while( string::npos != ( findPos = source.find( from, lastPos )))
  {
    newString.append( source, lastPos, findPos - lastPos );
    newString += to;
    lastPos = findPos + from.length();
  }

  // Care for the rest after last occurrence
  newString += source.substr( lastPos );

  source.swap( newString );
}


void ReplaceAllChar( string& source, const char* from, const string& to )
{
  string newString;
  newString.reserve( source.length() );  // avoids a few memory allocations

  string::size_type lastPos = 0;
  string::size_type findPos;

  while( string::npos != ( findPos = source.find( from, lastPos )))
  {
    newString.append( source, lastPos, findPos - lastPos );
    newString += to;
    lastPos = findPos+1;
  }

  // Care for the rest after last occurrence
  newString += source.substr( lastPos );

  source.swap( newString );
}


void toUpper(string &s){
  transform(s.begin(), s.end(),s.begin(), ::toupper);
}

void standardisation(string &var) {

  delFuell(var);
  delTitel(var);
  replaceNonAscii(var);
} //end standardisation

void preprocess(string& var) {
  toUpper(var);


} //end preprocess

/**
* checks  char
*/
bool invalidChar (char c)
{
  return !((c>=32 &&c <=126));//||(c==32)||(c==39)||(c==58)||(c==59)||(c==95));
}

bool invalidCharPunct (char c)
{
  return !((c <48)||(c>=57 && c <=65)||(c>90));
}

//void replaceUmlaut(char c1, char c2){}

void replaceNonAscii(string& var) {
  preprocess(var);
  ReplaceAllSubstr(var, "\"", "");

  ReplaceAllChar(var, "À", "A");
  ReplaceAllChar(var, "Á", "A");
  ReplaceAllChar(var, "Â", "A");
  ReplaceAllChar(var, "Ã", "A");
  ReplaceAllChar(var, "Å", "A");
  ReplaceAllChar(var, "Ä", "AE");
  ReplaceAllChar(var, "à", "A");
  ReplaceAllChar(var, "á", "A");
  ReplaceAllChar(var, "â", "A");
  ReplaceAllChar(var, "ã", "A");
  ReplaceAllChar(var, "å", "A");
  ReplaceAllChar(var, "ä", "AE");
  ReplaceAllChar(var, "Æ", "AE");
  ReplaceAllChar(var, "æ", "AE");

  ReplaceAllChar(var, "Ç", "C");
  ReplaceAllChar(var, "ç", "C");

  ReplaceAllChar(var, "Ð", "D");
  ReplaceAllChar(var, "ð", "D");

  ReplaceAllChar(var, "È", "E");
  ReplaceAllChar(var, "É", "E");
  ReplaceAllChar(var, "Ê", "E");
  ReplaceAllChar(var, "Ë", "E");
  ReplaceAllChar(var, "è", "E");
  ReplaceAllChar(var, "é", "E");
  ReplaceAllChar(var, "ê", "E");
  ReplaceAllChar(var, "ë", "E");

  ReplaceAllChar(var, "Ƒ", "F");
  ReplaceAllChar(var, "ƒ", "F");

  ReplaceAllChar(var, "Í", "I");
  ReplaceAllChar(var, "Ì", "I");
  ReplaceAllChar(var, "Î", "I");
  ReplaceAllChar(var, "Ï", "I");
  ReplaceAllChar(var, "í", "I");
  ReplaceAllChar(var, "ì", "I");
  ReplaceAllChar(var, "î", "I");
  ReplaceAllChar(var, "ï", "I");

  ReplaceAllChar(var, "Ñ", "N");
  ReplaceAllChar(var, "ñ", "N");

  ReplaceAllChar(var, "Ó", "O");
  ReplaceAllChar(var, "Ò", "O");
  ReplaceAllChar(var, "Ô", "O");
  ReplaceAllChar(var, "Õ", "O");
  ReplaceAllChar(var, "Ø", "O");
  ReplaceAllChar(var, "Ö", "OE");
  ReplaceAllChar(var, "ó", "O");
  ReplaceAllChar(var, "ò", "O");
  ReplaceAllChar(var, "ô", "O");
  ReplaceAllChar(var, "õ", "O");
  ReplaceAllChar(var, "ø", "O");
  ReplaceAllChar(var, "ö", "OE");

  ReplaceAllChar(var, "ß", "SS");
  ReplaceAllChar(var, "Ş", "S");
  ReplaceAllChar(var, "Š", "S");
  ReplaceAllChar(var, "ş", "S");
  ReplaceAllChar(var, "š", "S");

  ReplaceAllChar(var, "Ù", "U");
  ReplaceAllChar(var, "Ú", "U");
  ReplaceAllChar(var, "Û", "U");
  ReplaceAllChar(var, "Ü", "UE");
  ReplaceAllChar(var, "ú", "U");
  ReplaceAllChar(var, "ù", "U");
  ReplaceAllChar(var, "û", "U");
  ReplaceAllChar(var, "ü", "UE");

  ReplaceAllChar(var, "Ý", "Y");
  ReplaceAllChar(var, "Ỳ", "Y");
  ReplaceAllChar(var, "Ÿ", "Y");
  ReplaceAllChar(var, "ý", "Y");
  ReplaceAllChar(var, "ỳ", "Y");
  ReplaceAllChar(var, "ŷ", "Y");
  ReplaceAllChar(var, "ÿ", "Y");

  ReplaceAllChar(var, "Ž", "Z");
  ReplaceAllChar(var, "ž", "Z");

  //remove everything left, that is non ascii
  var.erase(remove_if(var.begin(),var.end(), invalidChar), var.end());
} //end replaceNonAscii

void delTitel(string& var) {
  preprocess(var);
  ReplaceAllSubstr(var, "BARONESSE ", " ");
  ReplaceAllSubstr(var, "BARONIN ", " ");
  ReplaceAllSubstr(var, "BARON ", " ");
  ReplaceAllSubstr(var, "BR ", " ");
  ReplaceAllSubstr(var, "DOKTOR ", " ");
  ReplaceAllSubstr(var, "DR ", " ");
  ReplaceAllSubstr(var, "DRIN ", " ");
  ReplaceAllSubstr(var, "FREIFRAU ", " ");
  ReplaceAllSubstr(var, "FREIHERRIN ", " ");
  ReplaceAllSubstr(var, "FREIHERR ", " ");
  ReplaceAllSubstr(var, "FREIIN ", " ");
  ReplaceAllSubstr(var, "FUERSTIN ", " ");
  ReplaceAllSubstr(var, "FUERST ", " ");
  ReplaceAllSubstr(var, "GRAEFIN ", " ");
  ReplaceAllSubstr(var, "GROSSHERZOEGIN ", " ");
  ReplaceAllSubstr(var, "GROSSHERZOG ", " ");
  ReplaceAllSubstr(var, "GROSSHERZOGIN ", " ");
  ReplaceAllSubstr(var, "HERZOEGIN ", " ");
  ReplaceAllSubstr(var, "HERZOG ", " ");
  ReplaceAllSubstr(var, "HERZOGIN ", " ");
  ReplaceAllSubstr(var, "KRONRINZESSIN ", " ");
  ReplaceAllSubstr(var, "KRONPRINZ ", " ");
  ReplaceAllSubstr(var, "KURPRINZESSIN ", " ");
  ReplaceAllSubstr(var, "KURPRINZ ", " ");
  ReplaceAllSubstr(var, "LANDGRAEFIN ", " ");
  ReplaceAllSubstr(var, "LANDGRAF ", " ");
  ReplaceAllSubstr(var, "MARKGRAEFIN ", " ");
  ReplaceAllSubstr(var, "MARKGRAF ", " ");
  ReplaceAllSubstr(var, "MED ", " ");
  ReplaceAllSubstr(var, "PD ", " ");
  ReplaceAllSubstr(var, "PRINZESSIN ", " ");
  ReplaceAllSubstr(var, "PRINZ ", " ");
  ReplaceAllSubstr(var, "PROFESSORIN ", " ");
  ReplaceAllSubstr(var, "PROFESSOR ", " ");
  ReplaceAllSubstr(var, "PROFIN ", " ");
  ReplaceAllSubstr(var, "PROF ", " ");
  ReplaceAllSubstr(var, "REICHGRAEFIN ", " ");
  ReplaceAllSubstr(var, "REICHGRAF ", " ");
  ReplaceAllSubstr(var, "REICHSFREIHERRIN ", " ");
  ReplaceAllSubstr(var, "REICHSFREIHERR ", " ");
  ReplaceAllSubstr(var, "REICHSVIKAR ", " ");
  ReplaceAllSubstr(var, "RITTER ", " ");
  ReplaceAllSubstr(var, "SR ", " ");
  ReplaceAllSubstr(var, "GRAF ", " ");
} //end delTitel

void delFuell(string & var) {
  preprocess(var);

  ReplaceAllSubstr(var, " AL ", " ");
  ReplaceAllSubstr(var, " AM ", " ");
  ReplaceAllSubstr(var, " AN ", " ");
  ReplaceAllSubstr(var, " AUF ", " ");
  ReplaceAllSubstr(var, " BEN ", " ");
  ReplaceAllSubstr(var, " DA ", " ");
  ReplaceAllSubstr(var, " DEL ", " ");
  ReplaceAllSubstr(var, " DEM ", " ");
  ReplaceAllSubstr(var, " DEN ", " ");
  ReplaceAllSubstr(var, " DER ", " ");
  ReplaceAllSubstr(var, " DES ", " ");
  ReplaceAllSubstr(var, " DI ", " ");
  ReplaceAllSubstr(var, " DOS ", " ");
  ReplaceAllSubstr(var, " DU ", " ");
  ReplaceAllSubstr(var, " EL ", " ");
  ReplaceAllSubstr(var, " EN ", " ");
  ReplaceAllSubstr(var, " ET ", " ");
  ReplaceAllSubstr(var, " LA ", " ");
  ReplaceAllSubstr(var, " LE ", " ");
  ReplaceAllSubstr(var, " L ", " ");
  ReplaceAllSubstr(var, " MC ", " ");
  ReplaceAllSubstr(var, " MAC ", " ");
  ReplaceAllSubstr(var, " MED ", " ");
  ReplaceAllSubstr(var, " O ", " ");
  ReplaceAllSubstr(var, " TER ", " ");
  ReplaceAllSubstr(var, " UND ", " ");
  ReplaceAllSubstr(var, " VAN ", " ");
  ReplaceAllSubstr(var, " VON ", " ");
  ReplaceAllSubstr(var, " VOR ", " ");
  ReplaceAllSubstr(var, " VOM ", " ");
  ReplaceAllSubstr(var, " V ", " ");
  ReplaceAllSubstr(var, " Y ", " ");
  ReplaceAllSubstr(var, " ZUM ", " ");
  ReplaceAllSubstr(var, " ZUR ", " ");
  ReplaceAllSubstr(var, " ZU ", " ");
  ReplaceAllSubstr(var, " DE ", " ");
  ReplaceAllSubstr(var, " D ", " ");
  ReplaceAllSubstr(var, " A ", " ");

} //end delFuell


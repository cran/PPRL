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


void replaceNonAscii(string& var) {
  preprocess(var);
  ReplaceAllSubstr(var, "\"", "");
  ReplaceAllSubstr(var, "Æ", "AE");
  ReplaceAllSubstr(var, "æ", "AE");
  ReplaceAllSubstr(var, "Ä", "AE");
  ReplaceAllSubstr(var, "ä", "AE");

  ReplaceAllSubstr(var, "Å", "A");
  ReplaceAllSubstr(var, "å", "A");
  ReplaceAllSubstr(var, "Â", "A");
  ReplaceAllSubstr(var, "â", "A");
  ReplaceAllSubstr(var, "À", "A");
  ReplaceAllSubstr(var, "à", "A");
  ReplaceAllSubstr(var, "Á", "A");
  ReplaceAllSubstr(var, "á", "A");

  ReplaceAllSubstr(var, "Ç", "C");
  ReplaceAllSubstr(var, "Ç", "C");

  ReplaceAllSubstr(var, "Ê", "E");
  ReplaceAllSubstr(var, "ê", "E");
  ReplaceAllSubstr(var, "È", "E");
  ReplaceAllSubstr(var, "è", "E");
  ReplaceAllSubstr(var, "É", "E");
  ReplaceAllSubstr(var, "é", "E");

  ReplaceAllSubstr(var, "Ï", "I");
  ReplaceAllSubstr(var, "ï", "I");
  ReplaceAllSubstr(var, "Î", "I");
  ReplaceAllSubstr(var, "î", "I");
  ReplaceAllSubstr(var, "Ì", "I");
  ReplaceAllSubstr(var, "ì", "I");
  ReplaceAllSubstr(var, "Í", "I");
  ReplaceAllSubstr(var, "í", "I");

  ReplaceAllSubstr(var, "Ö", "OE");
  ReplaceAllSubstr(var, "ö", "OE");
  ReplaceAllSubstr(var, "Ø", "O");
  ReplaceAllSubstr(var, "ø", "O");
  ReplaceAllSubstr(var, "Ô", "O");
  ReplaceAllSubstr(var, "ô", "O");
  ReplaceAllSubstr(var, "Ò", "O");
  ReplaceAllSubstr(var, "ò", "O");
  ReplaceAllSubstr(var, "Ó", "O");
  ReplaceAllSubstr(var, "ó", "O");

  ReplaceAllSubstr(var, "ß", "SS");
  ReplaceAllSubstr(var, "Ş", "S");
  ReplaceAllSubstr(var, "ş", "S");

  ReplaceAllSubstr(var, "ü", "UE");
  ReplaceAllSubstr(var, "Ü", "UE");
  ReplaceAllSubstr(var, "Ů", "U");
  ReplaceAllSubstr(var, "Û", "U");
  ReplaceAllSubstr(var, "û", "U");
  ReplaceAllSubstr(var, "Ù", "U");
  ReplaceAllSubstr(var, "ù", "U");
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



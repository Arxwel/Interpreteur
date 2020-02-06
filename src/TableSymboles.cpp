#include "../include/TableSymboles.h"

TableSymboles::TableSymboles() : m_table() {
}

SymboleValue * TableSymboles::chercheAjoute(const Symbole & s)
// si s est identique à un symbole valué déjà présent dans la table,
// on renvoie un pointeur sur ce symbole valué.
// Sinon, on insère un nouveau symbole valué correspondant à s
// et on renvoie un pointeur sur le nouveau symbole valué inséré.
{
  /*
  vector<SymboleValue*>::iterator i;
  i = m_table.begin();
  while (i < m_table.end() && (**i).getChaine() < s.getChaine()) i++;
  if (i == m_table.end() || (**i).getChaine() != s.getChaine()) // si pas trouvé...
    i = m_table.insert(i, new SymboleValue(s));
  return *i;
  */
  SymboleValue *symbole = new SymboleValue(s);
  for (SymboleValue *symboleval : m_table) {
    if (symbole->getChaine() == symboleval->getChaine()) {
      delete (symbole);
      return symboleval;
    }
  }
  m_table.push_back(symbole);
  return symbole;
}


ostream & operator<<(ostream & cout, const TableSymboles & ts)
// affiche ts sur cout
{
  cout << endl << "Contenu de la Table des Symboles Values :" << endl
          << "---------------------------------------" << endl << endl;
  for (unsigned int i = 0; i < ts.getTaille(); i++)
    cout << "  " << ts[i] << endl;
  cout << endl;
  return cout;
}

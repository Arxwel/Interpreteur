#include <stdlib.h>
#include "../include/ArbreAbstrait.h"
#include "../include/Symbole.h"
#include "../include/SymboleValue.h"
#include "../include/Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

void NoeudSeqInst::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    for (unsigned int i = 0; i < m_instructions.size(); i++) {
        m_instructions[i]->traduitEnCPP(cout,indentation);
        cout << endl;
    }
}
////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudAffectation::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    this->traduitEnCPP2(cout,indentation);
    cout <<";";
}

void NoeudAffectation::traduitEnCPP2(ostream & cout, unsigned int indentation) const {
    cout << setw(indentation) << "";
    cout << ((SymboleValue*) m_variable)->getChaine() << "";
    cout << " = ";
    if(*((SymboleValue*)m_expression) == "<ENTIER>") {
        cout << ((SymboleValue*) m_expression)->getChaine() << "";
    } else {
        m_expression->traduitEnCPP(cout, 0);
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

void NoeudOperateurBinaire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    SymboleValue *opg = (SymboleValue*)(this->m_operandeGauche);
    SymboleValue *opd = (SymboleValue*)(this->m_operandeDroit);
    string op(m_operateur.getChaine());

    if(op == "non") {
        this->traduitEnCPP2(cout,indentation);
    } else {
        cout << setw(indentation) << "";
        if(*(opg) == "<VARIABLE>" || *(opg) == "<CHAINE>" || *(opg) == "<ENTIER>") {
            cout << (opg->getChaine()) << "";
        } else {
            m_operandeGauche->traduitEnCPP(cout, 0);
        }

        if(op == "et") {
            op = "&&";
        } else if (op == "ou") {
            op = "||";
        }

        cout << " " << op << " ";
        if(*(opd) == "<VARIABLE>" || *(opd) == "<CHAINE>" || *(opd) == "<ENTIER>") {
            cout << (opd->getChaine()) << "";
        } else {
            m_operandeDroit->traduitEnCPP(cout, 0);
        }
    }
}

void NoeudOperateurBinaire::traduitEnCPP2(ostream & cout, unsigned int indentation) const {
    string op("!");
    SymboleValue *opg = (SymboleValue*)(this->m_operandeGauche);
    cout << setw(indentation) << "";
    cout << op;
        if(*(opg) == "<VARIABLE>" || *(opg) == "<CHAINE>" || *(opg) == "<ENTIER>") {
            cout << (opg->getChaine()) << "";
        } else {
            m_operandeGauche->traduitEnCPP(cout, 0);
        }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition == nullptr||m_condition->executer()) {
    m_sequence->executer();
  }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstSi::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "if (";
    m_condition->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout,indentation+4);
    cout << setw(indentation) << "" << "}";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstTantQue
////////////////////////////////////////////////////////////////////////////////
NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
  while (m_condition->executer()) {
      m_sequence->executer();
  }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstTantQue::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "while (";
    m_condition->traduitEnCPP(cout,0);
    cout << ") {" << endl;
    m_sequence->traduitEnCPP(cout,indentation+4);
    cout << setw(indentation) << "" << "}";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSiRiche
////////////////////////////////////////////////////////////////////////////////
NoeudInstSiRiche::NoeudInstSiRiche(vector<NoeudInstSi *> instructions) : m_inst(instructions){}

int NoeudInstSiRiche::executer() {
    for (NoeudInstSi * inst : m_inst) {
        if(inst->executer()) {
            return 0;
        }
    }
    return 0;
}

void NoeudInstSiRiche::ajoute(Noeud *inst) {
    m_inst.push_back((NoeudInstSi*)inst);
}

void NoeudInstSiRiche::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    m_inst[0]->traduitEnCPP(cout,indentation);
    for (unsigned int i = 1; i < m_inst.size(); i++) {
            cout << setw(indentation) << " else ";
            m_inst[i]->traduitEnCPP(cout,indentation);
    }
}
////////////////////////////////////////////////////////////////////////////////
// NoeudInstRepeter
////////////////////////////////////////////////////////////////////////////////
NoeudInstRepeter::NoeudInstRepeter(Noeud* condition, Noeud* sequence)
        : m_condition(condition), m_sequence(sequence) {
}

int NoeudInstRepeter::executer() {
  while (!m_condition->executer()) {
      m_sequence->executer();
  }
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstRepeter::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    cout << setw(indentation) << "" << "while (!(";
    m_condition->traduitEnCPP(cout,0);
    cout << ")) {" << endl;
    m_sequence->traduitEnCPP(cout,indentation+4);
    cout << setw(indentation) << "" << "}";
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstPour
////////////////////////////////////////////////////////////////////////////////
NoeudInstPour::NoeudInstPour(Noeud* affect1, Noeud* expression,Noeud* sequence, Noeud* affect2)
        : m_affect1(affect1), m_expression(expression),m_sequence(sequence), m_affect2(affect2) {
}

int NoeudInstPour::executer() {
    if(m_affect1 == nullptr or m_affect2 == nullptr) {
        while (m_expression->executer()) {
            m_sequence->executer();
        }
    } else {
        for (m_affect1->executer(); m_expression->executer() ; m_affect2->executer()) {
            m_sequence->executer();
        }
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstPour::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    if(m_affect1 == nullptr or m_affect2 == nullptr) {
        cout << setw(indentation) << "" << "while (";
        m_expression->traduitEnCPP(cout, 0);
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation + 4);
        cout << setw(indentation) << "" << "}";
    } else {
        cout << setw(indentation) << "" << "for (";
        m_affect1->traduitEnCPP(cout,0);
        m_expression->traduitEnCPP(cout, 0);
        cout << ";";
        m_affect2->traduitEnCPP2(cout,0);
        cout << ") {" << endl;
        m_sequence->traduitEnCPP(cout, indentation + 4);
        cout << setw(indentation) << "" << "}";
    }
}


////////////////////////////////////////////////////////////////////////////////
// NoeudInstEcrire
////////////////////////////////////////////////////////////////////////////////
NoeudInstEcrire::NoeudInstEcrire(vector<Noeud*> contenu)
        : m_contenu(contenu) {
}

int NoeudInstEcrire::executer() {
    for (Noeud* contenu : m_contenu) {
        if((typeid(*contenu) == typeid(SymboleValue)) && *((SymboleValue*)contenu) == "<CHAINE>") {
            SymboleValue *valchaine = (SymboleValue*)(contenu);
            cout << valchaine->getChaine().substr(1,valchaine->getChaine().length()-2) << " ";
        } else {
            cout << contenu->executer() << " ";
        }
    }
    cout << endl;
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstEcrire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    for (Noeud* contenu : m_contenu) {
        cout << setw(indentation) << "" << "cout << ";
            SymboleValue *valchaine = (SymboleValue*)(contenu);
            cout << valchaine->getChaine();
            cout << " << endl;" << endl;
    }
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstLire
////////////////////////////////////////////////////////////////////////////////
NoeudInstLire::NoeudInstLire(vector<Noeud*> contenu)
        : m_contenu(contenu) {
}

int NoeudInstLire::executer() {
    for (Noeud* contenu : m_contenu) {
        SymboleValue *valeurcontenu = (SymboleValue*)(contenu);
        int valvar;

        cout << "Saisie de la valeur de la variable " << valeurcontenu->getChaine() << " ";
        cin >> valvar;
        valeurcontenu->setValeur(valvar);
    }
    return 0; // La valeur renvoyée ne représente rien !
}

void NoeudInstLire::traduitEnCPP(ostream & cout, unsigned int indentation) const {
    for (Noeud* contenu : m_contenu) {
       cout << setw(indentation) << "" << "cin >> ";
       SymboleValue *valeurcontenu = (SymboleValue*)(contenu);
       cout << valeurcontenu->getChaine() << " >> endl;" << endl;
    }
}
#ifndef FIELDCONTAINER_CLASS_DECLARATION
#define FIELDCONTAINER_CLASS_DECLARATION

/** .


@author jbehr, Thu Jan  8 19:53:04 1998

*/

#include <list>
#include <iostream>
#include <fstream>

#include "ActionType.h"
#include "Field.h"

using namespace std;

class FieldContainer {

	friend class Field;

private:

	enum FieldKey { NAME_FIELD, PARENT_FIELD, LIBRARY_FIELD, 
									STRUCTURE_FIELD, POINTERFIELDTYPES_FIELD, 

									TYPE_FIELD, CARDINALITY_FIELD, VISIBILITY_FIELD, 
									DEFAULTVALUE_FIELD, ACCESS_FIELD, HEADER_FIELD, 
									UNKNOWN_FIELD
	};

	struct KeyDic {
		FieldKey  key;
		char      *name;
	};

	///
	static KeyDic _keyDic[];

	///
	static const char _filePrefix[];

	///
	static const char _descFileSuffix[];

	/// 
	static const char _decFileSuffix[];

	/// 
	static const char _inlFileSuffix[];

	///
	static const char _impFileSuffix[];

	///
	static const char _nil[];

	///
	static const char *_pointerFieldTypesName[];

	///
	static const char *_abstractName[];

  /// 
  char* _name;

  /// 
  char* _parentFieldContainer;

  /// 
  char* _description;

	///
	char* _library;

	///
	int _pointerFieldTypes;

	///
	int _abstract;

  ///
	list<Field> _fieldList;

	///
	FieldKey findFieldKey (const char *key);
	
	///
	void putField ( ofstream &out, const char *prefix,
								  FieldKey key, const char *value);

protected:

public:

  /// Class Constructor
  FieldContainer (void);

  /// Class Copy Constructor
  FieldContainer (FieldContainer &obj);

  /// Class Descructor
  virtual ~FieldContainer (void);

	/// get the pointerFieldTypes string for the given index
	static const char *pointerFieldTypesStr(int index);

	/// get the abstract string for the given index
	static const char *abstractStr(int index);

	/// clear the attributes
	virtual void clear(void);

  /// get method for attribute name
  virtual char* name (void) { return _name; }

  /// set method for attribute name
  virtual void setName (const char* name);

  /// get method for attribute parentFieldContainer
  virtual char* parentFieldContainer (void) { return _parentFieldContainer; }

  /// set method for attribute parentFieldContainer
  virtual void setParentFieldContainer (const char* parentFieldContainer);

  /// get method for attribute description
  virtual char* description (void) { return _description; }

  /// set method for attribute description
  virtual void setDescription (const char* description);

  /// get method for attribute library
  virtual char* library (void) { return _library; }

  /// set method for attribute library
  virtual void setLibrary (const char* library);
	
  /// get method for attribute pointerFieldTypes
  virtual int pointerFieldTypes (void) { return _pointerFieldTypes; }
	
  /// get method for attribute abstract
  virtual int abstract (void) { return _abstract; }

  /// set method for attribute pointerFieldTypes
  virtual void setPointerFieldTypes (int pointerFieldTypes) 
		{ _pointerFieldTypes = pointerFieldTypes; }

  /// set method for attribute abstract
  virtual void setAbstract (int abstract) 
		{ _abstract = abstract; }

  /// set method for attribute pointerFieldTypes
	virtual void setPointerFieldTypes (const char *str);

  /// set method for attribute pointerFieldTypes
	virtual void setAbstract (const char *str);

  ///
  list<Field> &fieldList(void) { return _fieldList; }

	///
	Field *getField(unsigned index);

  /// 
  virtual bool readDesc (const char *fileName = 0);

	///
	virtual bool writeTempl ( ofstream & out, char ** templ );

	///
	virtual bool writeDesc (const char *fileName = 0);

	///
	virtual bool writeCode ( bool base, bool fc );

	///
	virtual bool writeBaseCodeDec (const char *decFile);

	///
	virtual bool writeBaseCodeInl (const char *InlFile);

	///
	virtual bool writeBaseCodeImp ( const char *decFile, 
																	const char *impFile);

	///
	virtual bool writeCodeDec ( const char *decFile );																		
	///
	virtual bool writeCodeInl ( const char *InlFile );																		
	///
	virtual bool writeCodeImp ( const char *decFile, 
															const char *impFile );


	/// 
	static const char *filePrefix(void) { return _filePrefix; }

	/// 
	static const char *decFileSuffix(void) { return _decFileSuffix; }

	///
	static const char *inlFileSuffix(void) { return _inlFileSuffix; }

	///
	static const char *impFileSuffix(void) { return _impFileSuffix; }

  /// create a new field and returns the index;
	int newField (const Field &newField);

	///
	void delField(unsigned index);

	///
	bool moveFieldLeft (unsigned index);

	///
	bool moveFieldRight (unsigned index);

	///
	unsigned numOfFields(void) { return _fieldList.size(); }


};

typedef FieldContainer* FieldContainerP;

#endif // FIELDCONTAINER_CLASS_DECLARATION

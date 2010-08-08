#include <QObject>

class QString;

class ClassInfo: public QObject
{
  Q_OBJECT
  Q_PROPERTY(QString name READ name)
  Q_PROPERTY(QString privName READ privName)
  Q_PROPERTY(QString baseName READ baseName)
  Q_PROPERTY(QObject* parent READ parent)
  Q_PROPERTY(QObject* content READ content)
  Q_PROPERTY(QString parentBaseName READ parentBaseName)
  Q_PROPERTY(QString parentName READ parentName)
  Q_PROPERTY(QString group READ group)
  Q_PROPERTY(QString brief READ brief)
  Q_PROPERTY(QString listOf READ listOf)
  Q_PROPERTY(bool internal READ internal)
  Q_PROPERTY(bool doxOnly READ doxOnly)
  Q_PROPERTY(bool hiddenStruct READ hiddenStruct)

  Q_PROPERTY(QString dox READ dox)
  Q_PROPERTY(QString members_h READ members_h)
  Q_PROPERTY(QString private_custom_definitions_c READ private_custom_definitions_c)
  Q_PROPERTY(QString private_methods_declarations_h READ private_methods_declarations_h)
  Q_PROPERTY(QString private_methods_definitions_c READ private_methods_definitions_c)
  Q_PROPERTY(QString public_h READ public_h)
  Q_PROPERTY(QString private_h READ private_h)
  Q_PROPERTY(QString public_methods_declarations_h READ public_methods_declarations_h)
  Q_PROPERTY(QString public_methods_definitions_c READ public_methods_definitions_c)

  public:
    ClassInfo( const QString& name, const QString& dir, bool isnew = false )
      : base(name), directory(dir),
        isInternal(false), isNew(isnew),
        autotemplates(true), hiddenstruct(true), list(false),
        m_parent(NULL), m_content(NULL)
    {
      parseDoxyfile();
      //parseSourceFiles(); FIXME This is not working
    }

    /* Public property functions start */
    /// Get the class full public name
    QString name() const { return "oy" + base + "_s"; }
    /// Get the class full private name
    QString privName() const { return "oy" + base + "_s_"; }
    /// Get the class name without any prefix/suffix
    QString baseName() const { return base; }
    /// Get the parent class
    QObject* parent() const { return static_cast<QObject*>(m_parent); }
    /// Get the content class
    QObject* content() const { return static_cast<QObject*>(m_content); }
    /// Set the parent class
    void setParent( ClassInfo* ci ) { m_parent = ci; }
    /// Set the content class
    void setContent( ClassInfo* ci ) { m_content = ci; }
    /// Get the baseName of the parent class
    QString parentBaseName() const { return parentBase; }
    /// Get the full name of the parent class
    QString parentName() const {
      if (parentBase.isEmpty())
        return "";
      else
        return "oy" + parentBase + "_s";
    }
    /// Get the group this class belongs to
    QString group() const { return groupName; }
    /// Get the class brief description
    QString brief() const { return doxyBrief; }
    /// Get the class of the list objects
    QString listOf() const {
      QString s( base );
      if (list) {
        s.chop(1);
        s = "oy" + s + "_s";
      } else
        s = "";
      return s;
    }
    /// True if this is an internal(not public) class
    bool internal() const { return isInternal; }
    /// True if this is a new class (with only a .dox file)
    bool doxOnly() const { return isNew; }

    /// Get the doxygen file
    QString dox() const { return base + ".dox"; }
    /// Get the source file members.h
    QString members_h() const { return base + "." + "members.h"; }
    /// Get the source file private_custom_definitions.c
    QString private_custom_definitions_c() const { return base + "." + "private_custom_definitions.c"; }
    /// Get the source file private_methods_declarations.h
    QString private_methods_declarations_h() const { return base + "." + "private_methods_declarations.h"; }
    /// Get the source file private_methods_definitions.c
    QString private_methods_definitions_c() const { return base + "." + "private_methods_definitions.c"; }
    /// Get the source file public.h
    QString public_h() const { return base + "." + "public.h"; }
    /// Get the source file private.h
    QString private_h() const { return base + "." + "private.h"; }
    /// Get the source file public_methods_declarations.h
    QString public_methods_declarations_h() const { return base + "." + "public_methods_declarations.h"; }
    /// Get the source file public_methods_definitions.c
    QString public_methods_definitions_c() const { return base + "." + "public_methods_definitions.c"; }
    /* Public property functions stop */

    /// Wether templates for this should be automaticly created
    bool createTemplates() const { return autotemplates; }
    /// Wether templates for this should be created using "hidden struct"
    bool hiddenStruct() const { return hiddenstruct; }

    static QList<ClassInfo*> getAllClasses( const QString& directory );

  private:
    QString base;           ///< The class name without any prefix/suffix
    QString parentBase;     ///< The base name of the parent class
    QString groupName;      ///< The group this class belongs to
    QString doxyBrief;      ///< The doxygen class brief description
    QString directory;      ///< Where the class source files live
    bool isInternal;        ///< True if this is an internal(not public) class
    bool isNew;             ///< True if this is a new class (with only a .dox file)
    bool autotemplates;     ///< True if templates should be created automaticly for this class
    bool hiddenstruct;      ///< True if templates should be created using "hidden struct"
    bool list;              ///< True if this class is a special "list" class
    ClassInfo* m_parent;    ///< A pointer to the parent class info
    ClassInfo* m_content;   ///< A pointer to the content class info, if this is a "list" class

    void parseDoxyfile();
    void parseSourceFiles();
};

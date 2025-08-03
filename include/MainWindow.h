// ============== MainWindow.h ==============
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QPushButton>
#include <QTableWidget>
#include <QDateEdit>
#include <QGroupBox>
#include <QLabel>
#include <QStatusBar>
#include <QSplitter>
#include "moviedatabase.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void addMovie();
    void searchMovies();
    void clearSearch();
    void refreshTable();
    void editMovie();
    void deleteMovie();                  
    void onTableDoubleClicked(int row, int column);  
private:
    void setupUI();
    void setupAddMovieForm();
    void setupSearchPanel();
    void setupMovieTable();
    void updateMovieTable(const QVector<Movie>& movies);
    void clearAddForm();
    void populateEditForm(const Movie& movie); 
    void showStatusMessage(const QString& message, int timeout = 3000);

    // UI Components
    QWidget* m_centralWidget;
    QSplitter* m_mainSplitter;
    
    // Add Movie Form
    QGroupBox* m_addMovieGroup;
    QLineEdit* m_movieNameEdit;
    QSpinBox* m_yearSpinBox;
    QTextEdit* m_notesEdit;
    QCheckBox* m_favoriteCheckBox;
    QPushButton* m_addButton;
    
    // Search Panel
    QGroupBox* m_searchGroup;
    QLineEdit* m_searchNameEdit;
    QDateEdit* m_startDateEdit;
    QDateEdit* m_endDateEdit;
    QCheckBox* m_favoritesOnlyCheckBox;
    QPushButton* m_searchButton;
    QPushButton* m_clearSearchButton;
    
    // Movie Display
    QTableWidget* m_movieTable;
    QPushButton* m_editButton;        
    QPushButton* m_deleteButton;
    QLabel* m_statusLabel;
    
    // Data
    MovieDatabase* m_database;
    QVector<Movie> m_currentMovies; 
    int m_editingIndex;
};

#endif // MAINWINDOW_H





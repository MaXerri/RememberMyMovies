
// ============== MainWindow.cpp ==============
#include "MainWindow.h"
#include <QApplication>
#include <QMessageBox>
#include <QHeaderView>
#include <QDate>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_database(new MovieDatabase("data/movies.csv")), m_editingIndex(-1)
{
    setupUI();
    
    // Load existing movies
    if (!m_database->loadFromCsv()) {
        showStatusMessage("Error loading movies: " + m_database->getLastError());
    } else {
        showStatusMessage(QString("Loaded %1 movies").arg(m_database->getMovieCount()));
        refreshTable();
    }
}

MainWindow::~MainWindow()
{
    delete m_database;
}

void MainWindow::setupUI()
{
    setWindowTitle("Movie Review Manager");
    setMinimumSize(1000, 700);
    
    // Create central widget and main layout
    m_centralWidget = new QWidget;
    setCentralWidget(m_centralWidget);
    
    // Create main splitter for resizable panels
    m_mainSplitter = new QSplitter(Qt::Horizontal);
    
    // Setup components
    setupAddMovieForm();
    setupSearchPanel();
    
    // Create left panel with forms
    QWidget* leftPanel = new QWidget;
    QVBoxLayout* leftLayout = new QVBoxLayout(leftPanel);
    leftLayout->addWidget(m_addMovieGroup);
    leftLayout->addWidget(m_searchGroup);
    leftLayout->addStretch(); // Push everything to top
    
    // Add to splitter
    m_mainSplitter->addWidget(leftPanel);
    
    // Create right panel with table and buttons
    QWidget* rightPanel = new QWidget;
    QVBoxLayout* rightLayout = new QVBoxLayout(rightPanel);
    
    // Table action buttons
    QHBoxLayout* tableButtonLayout = new QHBoxLayout;
    m_editButton = new QPushButton("Edit Selected");
    m_deleteButton = new QPushButton("Delete Selected");
    m_deleteButton->setStyleSheet("background-color: #dc3545;");
    setupMovieTable();
    
    tableButtonLayout->addWidget(m_editButton);
    tableButtonLayout->addWidget(m_deleteButton);
    tableButtonLayout->addStretch();
    
    rightLayout->addLayout(tableButtonLayout);
    rightLayout->addWidget(m_movieTable);
    
    m_mainSplitter->addWidget(rightPanel);
    m_mainSplitter->setSizes({350, 650}); // Set initial sizes
    
    // Main layout
    QHBoxLayout* mainLayout = new QHBoxLayout(m_centralWidget);
    mainLayout->addWidget(m_mainSplitter);
    
    // Status bar
    statusBar()->showMessage("Ready");
    
    // Apply modern styling
    setStyleSheet(R"(
        QMainWindow {
            background-color: #f5f5f5;
        }
        QGroupBox {
            font-weight: bold;
            border: 2px solid #cccccc;
            border-radius: 8px;
            margin-top: 1ex;
            padding-top: 10px;
        }
        QGroupBox::title {
            subcontrol-origin: margin;
            left: 10px;
            padding: 0 5px 0 5px;
        }
        QPushButton {
            background-color: #007AFF;
            color: white;
            border: none;
            border-radius: 6px;
            padding: 8px 16px;
            font-weight: bold;
        }
        QPushButton:hover {
            background-color: #0056CC;
        }
        QPushButton:pressed {
            background-color: #004499;
        }
        QLineEdit, QSpinBox, QTextEdit, QDateEdit {
            border: 1px solid #cccccc;
            border-radius: 4px;
            padding: 6px;
            background-color: white;
        }
        QLineEdit:focus, QSpinBox:focus, QTextEdit:focus, QDateEdit:focus {
            border-color: #007AFF;
        }
        QTableWidget {
            gridline-color: #e0e0e0;
            background-color: white;
            alternate-background-color: #f9f9f9;
        }
        QTableWidget::item {
            padding: 4px;
        }
        QTableWidget::item:selected {
            background-color: #007AFF;
            color: white;
        }
    )");
}

void MainWindow::setupAddMovieForm()
{
    m_addMovieGroup = new QGroupBox("Add New Movie");
    QFormLayout* formLayout = new QFormLayout(m_addMovieGroup);
    
    // Movie name
    m_movieNameEdit = new QLineEdit;
    m_movieNameEdit->setPlaceholderText("Enter movie name...");
    formLayout->addRow("Movie Name*:", m_movieNameEdit);
    
    // Year
    m_yearSpinBox = new QSpinBox;
    m_yearSpinBox->setRange(1900, QDate::currentDate().year() + 5);
    m_yearSpinBox->setValue(QDate::currentDate().year());
    formLayout->addRow("Year:", m_yearSpinBox);
    
    // Notes
    m_notesEdit = new QTextEdit;
    m_notesEdit->setMaximumHeight(80);
    m_notesEdit->setPlaceholderText("Your thoughts about this movie...");
    formLayout->addRow("Notes:", m_notesEdit);
    
    // Favorite checkbox
    m_favoriteCheckBox = new QCheckBox("Mark as favorite");
    formLayout->addRow("", m_favoriteCheckBox);
    
    // Add button
    m_addButton = new QPushButton("Add Movie");
    formLayout->addRow("", m_addButton);
    
    // Edit mode label (initially hidden)
    QLabel* editLabel = new QLabel("✏️ Editing Mode - Click 'Update Movie' to save changes");
    editLabel->setObjectName("editLabel");
    editLabel->setStyleSheet("color: #FF6B35; font-weight: bold; padding: 5px;");
    editLabel->setVisible(false);
    formLayout->addRow("", editLabel);
    
    // Connect signals
    connect(m_addButton, &QPushButton::clicked, this, &MainWindow::addMovie);
    connect(m_movieNameEdit, &QLineEdit::returnPressed, this, &MainWindow::addMovie);
}

void MainWindow::setupSearchPanel()
{
    m_searchGroup = new QGroupBox("Search Movies");
    QFormLayout* searchLayout = new QFormLayout(m_searchGroup);
    
    // Search by name
    m_searchNameEdit = new QLineEdit;
    m_searchNameEdit->setPlaceholderText("Exact movie name...");
    searchLayout->addRow("Movie Name:", m_searchNameEdit);
    
    // Date range
    m_startDateEdit = new QDateEdit;
    m_startDateEdit->setDate(QDate::currentDate().addDays(-30));
    m_startDateEdit->setCalendarPopup(true);
    searchLayout->addRow("From Date:", m_startDateEdit);
    
    m_endDateEdit = new QDateEdit;
    m_endDateEdit->setDate(QDate::currentDate());
    m_endDateEdit->setCalendarPopup(true);
    searchLayout->addRow("To Date:", m_endDateEdit);
    
    // Favorites only
    m_favoritesOnlyCheckBox = new QCheckBox("Favorites only");
    searchLayout->addRow("", m_favoritesOnlyCheckBox);
    
    // Search buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout;
    m_searchButton = new QPushButton("Search");
    m_clearSearchButton = new QPushButton("Show All");
    m_clearSearchButton->setStyleSheet("background-color: #6c757d;");
    
    buttonLayout->addWidget(m_searchButton);
    buttonLayout->addWidget(m_clearSearchButton);
    searchLayout->addRow("", buttonLayout);
    
    // Connect signals
    connect(m_searchButton, &QPushButton::clicked, this, &MainWindow::searchMovies);
    connect(m_clearSearchButton, &QPushButton::clicked, this, &MainWindow::clearSearch);
    connect(m_searchNameEdit, &QLineEdit::returnPressed, this, &MainWindow::searchMovies);
}

void MainWindow::setupMovieTable()
{
    m_movieTable = new QTableWidget;
    m_movieTable->setColumnCount(5);
    
    QStringList headers = {"Movie Name", "Year", "Date Added", "Notes", "Favorite"};
    m_movieTable->setHorizontalHeaderLabels(headers);
    
    // Configure table appearance
    m_movieTable->setAlternatingRowColors(true);
    m_movieTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_movieTable->setSortingEnabled(true);
    m_movieTable->verticalHeader()->setVisible(false);
    
    // Set column widths
    QHeaderView* header = m_movieTable->horizontalHeader();
    header->setStretchLastSection(false);
    header->resizeSection(0, 200); // Movie Name
    header->resizeSection(1, 80);  // Year
    header->resizeSection(2, 120); // Date Added
    header->resizeSection(3, 300); // Notes
    header->resizeSection(4, 80);  // Favorite
    
    // Connect table signals
    connect(m_movieTable, &QTableWidget::cellDoubleClicked, this, &MainWindow::onTableDoubleClicked);
    connect(m_editButton, &QPushButton::clicked, this, &MainWindow::editMovie);
    connect(m_deleteButton, &QPushButton::clicked, this, &MainWindow::deleteMovie);
}

void MainWindow::addMovie()
{
    // Validate input
    QString movieName = m_movieNameEdit->text().trimmed();
    if (movieName.isEmpty()) {
        QMessageBox::warning(this, "Input Error", "Please enter a movie name.");
        m_movieNameEdit->setFocus();
        return;
    }
    
    if (m_editingIndex >= 0) {
        // Update existing movie
        Movie updatedMovie(movieName, 
                          m_yearSpinBox->value(),
                          m_notesEdit->toPlainText().trimmed(),
                          m_favoriteCheckBox->isChecked());
        
        // Keep original date added
        Movie originalMovie = m_currentMovies[m_editingIndex];
        updatedMovie.setDateAdded(originalMovie.getDateAdded());
        
        // Update in database
        m_database->updateMovie(m_editingIndex, updatedMovie);
        
        showStatusMessage(QString("Updated movie: %1").arg(movieName));
        
        // Exit edit mode
        m_editingIndex = -1;
        m_addButton->setText("Add Movie");
        findChild<QLabel*>("editLabel")->setVisible(false);
    } else {
        // Create new movie
        Movie movie(movieName, 
                    m_yearSpinBox->value(),
                    m_notesEdit->toPlainText().trimmed(),
                    m_favoriteCheckBox->isChecked());
        
        // Add to database
        m_database->addMovie(movie);
        
        showStatusMessage(QString("Added movie: %1").arg(movieName));
    }
    
    // Update display
    refreshTable();
    clearAddForm();
}

void MainWindow::searchMovies()
{
    QVector<Movie> results = m_database->getAllMovies();
    
    // Apply name filter
    QString searchName = m_searchNameEdit->text().trimmed();
    if (!searchName.isEmpty()) {
        results = m_database->searchByName(searchName);
    }
    
    // Apply date range filter
    if (!results.isEmpty()) {
        QDate startDate = m_startDateEdit->date();
        QDate endDate = m_endDateEdit->date();
        
        QVector<Movie> dateFiltered;
        for (const Movie& movie : results) {
            QDate movieDate = movie.getDateAdded();
            if (movieDate >= startDate && movieDate <= endDate) {
                dateFiltered.append(movie);
            }
        }
        results = dateFiltered;
    }
    
    // Apply favorites filter
    if (m_favoritesOnlyCheckBox->isChecked() && !results.isEmpty()) {
        QVector<Movie> favoriteFiltered;
        for (const Movie& movie : results) {
            if (movie.isFavorite()) {
                favoriteFiltered.append(movie);
            }
        }
        results = favoriteFiltered;
    }
    
    updateMovieTable(results);
    showStatusMessage(QString("Found %1 movies").arg(results.size()));
}

void MainWindow::clearSearch()
{
    m_searchNameEdit->clear();
    m_startDateEdit->setDate(QDate::currentDate().addDays(-30));
    m_endDateEdit->setDate(QDate::currentDate());
    m_favoritesOnlyCheckBox->setChecked(false);
    
    refreshTable();
    showStatusMessage("Showing all movies");
}

void MainWindow::refreshTable()
{
    m_currentMovies = m_database->getAllMovies();
    updateMovieTable(m_currentMovies);
}

void MainWindow::updateMovieTable(const QVector<Movie>& movies)
{
    m_currentMovies = movies; // Store current view
    m_movieTable->setRowCount(movies.size());
    
    for (int i = 0; i < movies.size(); ++i) {
        const Movie& movie = movies[i];
        
        m_movieTable->setItem(i, 0, new QTableWidgetItem(movie.getName()));
        m_movieTable->setItem(i, 1, new QTableWidgetItem(QString::number(movie.getYear())));
        m_movieTable->setItem(i, 2, new QTableWidgetItem(movie.getDateAdded().toString("yyyy-MM-dd")));
        m_movieTable->setItem(i, 3, new QTableWidgetItem(movie.getNotes()));
        m_movieTable->setItem(i, 4, new QTableWidgetItem(movie.isFavorite() ? "★" : ""));
    }
    
    // Auto-resize the notes column to fit content
    m_movieTable->resizeRowsToContents();
}

void MainWindow::editMovie()
{
    int currentRow = m_movieTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "No Selection", "Please select a movie to edit.");
        return;
    }
    
    onTableDoubleClicked(currentRow, 0);
}

void MainWindow::deleteMovie()
{
    int currentRow = m_movieTable->currentRow();
    if (currentRow < 0) {
        QMessageBox::information(this, "No Selection", "Please select a movie to delete.");
        return;
    }
    
    if (currentRow >= m_currentMovies.size()) {
        QMessageBox::warning(this, "Error", "Invalid selection.");
        return;
    }
    
    Movie movieToDelete = m_currentMovies[currentRow];
    
    // Confirm deletion
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Confirm Deletion", 
                                 QString("Are you sure you want to delete:\n\n%1 (%2)?")
                                 .arg(movieToDelete.getName()).arg(movieToDelete.getYear()),
                                 QMessageBox::Yes | QMessageBox::No);
    
    if (reply == QMessageBox::Yes) {
        // Find the movie in the full database and remove it
        m_database->deleteMovie(movieToDelete);
        
        // Update display
        refreshTable();
        
        showStatusMessage(QString("Deleted movie: %1").arg(movieToDelete.getName()));
    }
}

void MainWindow::onTableDoubleClicked(int row, int column)
{
    Q_UNUSED(column)
    
    if (row < 0 || row >= m_currentMovies.size()) {
        return;
    }
    
    Movie movieToEdit = m_currentMovies[row];
    populateEditForm(movieToEdit);
    
    // Set edit mode
    m_editingIndex = row;
    m_addButton->setText("Update Movie");
    findChild<QLabel*>("editLabel")->setVisible(true);
    
    showStatusMessage("Editing mode: Make changes and click 'Update Movie'");
}

void MainWindow::populateEditForm(const Movie& movie)
{
    m_movieNameEdit->setText(movie.getName());
    m_yearSpinBox->setValue(movie.getYear());
    m_notesEdit->setPlainText(movie.getNotes());
    m_favoriteCheckBox->setChecked(movie.isFavorite());
    
    m_movieNameEdit->setFocus();
}

void MainWindow::clearAddForm()
{
    m_movieNameEdit->clear();
    m_yearSpinBox->setValue(QDate::currentDate().year());
    m_notesEdit->clear();
    m_favoriteCheckBox->setChecked(false);
    m_movieNameEdit->setFocus();
    
    // Reset edit mode
    m_editingIndex = -1;
    m_addButton->setText("Add Movie");
    findChild<QLabel*>("editLabel")->setVisible(false);
}

void MainWindow::showStatusMessage(const QString& message, int timeout)
{
    statusBar()->showMessage(message, timeout);
}





#include <algorithm>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <minmax.h>
#include <string>
#include <unordered_map>
#include <vector>

namespace Text
{
	inline auto ToUpper(std::string &str) -> void
	{
		for (int i = 0; str[i] != '\0'; i++)
		{
			if (str[i] >= 'a' && str[i] <= 'z')
				str[i] = str[i] - 32;
		}
	}

	inline auto ToLower(std::string &str) -> void
	{
		for (int i = 0; str[i] != '\0'; i++)
		{
			if (str[i] >= 'A' && str[i] <= 'Z')
				str[i] = str[i] + 32;
		}
	}

	inline void RemoveExtraSpaces(std::string &str)
	{
		str.erase(str.begin(), std::find_if(str.begin(), str.end(), [](int c) { return !std::isspace(c); }));

		str.erase(std::find_if(str.rbegin(), str.rend(), [](int c) { return !std::isspace(c); }).base(), str.end());

		auto last =
			std::unique(str.begin(), str.end(), [](char a, char b) { return std::isspace(a) && std::isspace(b); });
		str.erase(last, str.end());
	}
}

class Book
{
   public:
	Book()
	{
	}

	Book(int bookID) : m_BookID(bookID)
	{
		m_Title	 = "Untitled Book";
		m_Year	 = "0000";
		m_Author = "None";
	}

	~Book()
	{
	}

	const std::string &GetTitle()
	{
		return m_Title;
	}

	void SetTitle(const std::string title)
	{
		m_Title = title;
	}

	const std::string &GetAuthor()
	{
		return m_Author;
	}

	void SetAuthor(const std::string author)
	{
		m_Author = author;
	}

	const std::string &GetYear()
	{
		return m_Year;
	}

	void SetYear(const std::string year)
	{
		m_Year = year;
	}

	const int &GetBookID()
	{
		return m_BookID;
	}

   private:
	std::string m_Title;
	std::string m_Author;
	std::string m_Year;
	int			m_BookID;
};

class BookManager
{
   public:
	static BookManager &GetInstance()
	{
		static BookManager m_Instance;
		return m_Instance;
	}

   private:
	BookManager()
	{
	}
	BookManager(BookManager const &)			= delete;
	auto operator=(BookManager const &) -> void = delete;

   public:
	void Books(const std::function<void(Book *)> &fn)
	{
		for (Book *book : m_Books)
		{
			fn(book);
		}
	}

	Book *NewBook()
	{
		Book *newBook = new Book(m_CurrentBookID++);

		m_Books.emplace_back(newBook);

		return newBook;
	}

	Book *GetBook(Book book, bool titleOnly = false)
	{
		auto it = std::find_if(m_Books.begin(), m_Books.end(),
							   [&](Book *b)
							   {
								   if (titleOnly)
								   {
									   std::string bookTitle = book.GetTitle();
									   std::string bTitle	 = b->GetTitle();

									   Text::ToLower(bookTitle);
									   Text::RemoveExtraSpaces(bookTitle);

									   Text::ToLower(bTitle);
									   Text::RemoveExtraSpaces(bTitle);

									   return bTitle == bookTitle;
								   }

								   return book.GetAuthor() == b->GetAuthor() && book.GetTitle() == b->GetTitle() &&
										  book.GetYear() == b->GetYear();
							   });

		if (it != m_Books.end())
		{
			return *it;
		}

		return nullptr;
	}

	bool IsBookExisting(std::string title)
	{
		auto it = std::find_if(m_Books.begin(), m_Books.end(),
							   [&](Book *b)
							   {
								   std::string bTitle = b->GetTitle();

								   Text::ToLower(title);
								   Text::RemoveExtraSpaces(title);

								   Text::ToLower(bTitle);
								   Text::RemoveExtraSpaces(bTitle);

								   return bTitle == title;
							   });

		if (it != m_Books.end())
		{
			return true;
		}

		return false;
	}

	void RemoveBook(Book book)
	{
		auto it = std::find_if(m_Books.begin(), m_Books.end(),
							   [&](Book *b) {
								   return book.GetAuthor() == b->GetAuthor() && book.GetTitle() == b->GetTitle() &&
										  book.GetYear() == b->GetYear();
							   });

		if (it != m_Books.end())
		{
			Book *b = *it;
			delete b;

			m_Books.erase(it);
		}
	}

	void Clear()
	{
		for (auto it = m_Books.begin(); it != m_Books.end();)
		{
			delete *it;
			it = m_Books.erase(it);
		}
	}

	int GetBookCount()
	{
		return m_Books.size();
	}

	int GetCurrentBookID()
	{
		return m_CurrentBookID;
	}

   private:
	std::vector<Book *> m_Books			= {};
	int					m_CurrentBookID = 0;
};

BookManager &GetBookManager()
{
	return BookManager::GetInstance();
}

void ProcessBookAdd()
{
	std::cout << "Enter information for new book of index " << GetBookManager().GetCurrentBookID() << ":\n";

	std::string title, author, yearStr;
	int			yearInt;

	do
	{
		std::cout << "Enter Book Title [3-32 Characters]: ";
		std::cin.ignore();
		std::getline(std::cin, title);

		if (title.size() < 3 || title.size() > 32)
		{
			std::cout << "A Book Title must be at least 3 and 32 characters in width!\n\n";
		}
		else if (GetBookManager().IsBookExisting(title))
		{
			std::cout << "That book already exists!\n\n";
		}

	} while (title.size() < 3 || title.size() > 32 || GetBookManager().IsBookExisting(title));

	do
	{
		std::cout << "Enter Book Author [1-32 Characters]: ";
		std::getline(std::cin, author);

		if (author.size() < 1 || author.size() > 32)
		{
			std::cout << "A Book Author's name must be at least 1 and 32 characters in width!\n\n";
		}
	} while (author.size() < 1 || author.size() > 32);

	do
	{
		std::cout << "Enter Year [1600-2024]: ";
		std::cin >> yearStr;

		yearInt = std::atoi(yearStr.c_str());

		if (yearInt < 1600 || yearInt > 2024)
		{
			std::cout << "A Book's Year should only be between 1600 and 2024!\n";
		}
	} while (yearInt < 1600 || yearInt > 2024);

	Book *book = GetBookManager().NewBook();
	book->SetTitle(title);
	book->SetAuthor(author);
	book->SetYear(yearStr);

	std::cout << "Book has been added!\n\n";
}

void ProcessListBooks()
{
	int colWidth1 = 38;
	int colWidth2 = 38;
	int colWidth3 = 10;

	std::cout << "Here are the books so far:\n";

	if (!GetBookManager().GetBookCount())
	{
		std::cout << "There are currently no books stored in me\n\n";
		return;
	}

	std::cout << std::left << std::setw(colWidth1) << "Title" << std::setw(colWidth2) << "Author"
			  << std::setw(colWidth3) << "Year" << std::endl;

	std::cout << std::setfill('-') << std::setw(colWidth1 + colWidth2 + colWidth3) << "" << std::setfill(' ')
			  << std::endl;

	GetBookManager().Books(
		[&](Book *book)
		{
			std::cout << std::left << std::setw(colWidth1) << book->GetTitle() << std::setw(colWidth2)
					  << book->GetAuthor() << std::setw(colWidth3) << book->GetYear() << std::endl;
		});

	std::cout << "\n";
}

void ProcessSearchBooks()
{
	std::string title = "";
	std::cout << "Enter Book Title to Search For: ";
	std::cin.ignore();
	std::getline(std::cin, title);

	Book query;
	query.SetTitle(title);

	Book *book = GetBookManager().GetBook(query, true);
	if (!book)
	{
		std::cout << "\nBook '" << title << "' not found!\n";
		return;
	}

	std::cout << "\nInformation for Book '" << title << "':\n";
	std::cout << "Title: " << book->GetTitle() << "\n";
	std::cout << "Author: " << book->GetAuthor() << "\n";
	std::cout << "Year: " << book->GetYear() << "\n\n";
}

int main()
{
	system("cls");
	std::cout << "\n< < Coffee's Book Manager > >\n";

	for (;;)
	{
		std::cout << "Please choose an option:\n"
					 "1 - Add Book\n"
					 "2 - List Books\n"
					 "3 - Search Book\n"
					 "4 - Exit\n"
					 "> ";

		char in;
		std::cin >> in;
		switch (in)
		{
			case '1':
			{
				ProcessBookAdd();
				break;
			}
			case '2':
			{
				ProcessListBooks();
				break;
			}
			case '3':
			{
				ProcessSearchBooks();
				break;
			}
			case '4':
			{
				std::cout << "Farewell & See U Soon!\n";
				goto done;
			}
			default:
			{
				std::cout << "Invalid Option!\n\n";
				break;
			}
		}
	}

done:

	GetBookManager().Clear();

	std::cout << "Exiting <3\n";

	return 0;
}
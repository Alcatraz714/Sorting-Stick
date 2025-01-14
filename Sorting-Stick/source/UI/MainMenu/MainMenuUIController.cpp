#include "UI/MainMenu/MainMenuUIController.h"
#include "Main/GameService.h"
#include "Graphics/GraphicService.h"
#include "Sound/SoundService.h"
#include "Event/EventService.h"
#include "Global/Config.h"
#include "Global/ServiceLocator.h"

namespace UI
{
    namespace MainMenu
    {
        using namespace Global;
        using namespace Main;
        using namespace UIElement;
        using namespace Sound;
        using namespace Graphics;

        MainMenuUIController::MainMenuUIController()
        {
            createImage();
            createButtons();
        }

        MainMenuUIController::~MainMenuUIController()
        {
            destroy();
        }

        void MainMenuUIController::initialize()
        {
            initializeBackgroundImage();
            initializeButtons();
            registerButtonCallback();
        }

        void MainMenuUIController::createImage()
        {
            background_image = new ImageView();
        }

        void MainMenuUIController::createButtons()
        {
            bubble_sort_button = new ButtonView();
            insertion_sort_button = new ButtonView();
            selection_sort_button = new ButtonView();
            merge_sort_button = new ButtonView();
            quick_sort_button = new ButtonView();
            radix_sort_button = new ButtonView();
            quit_button = new ButtonView();
        }

        void MainMenuUIController::initializeBackgroundImage()
        {
            GraphicService* graphic_service = ServiceLocator::getInstance()->getGraphicService();

            background_image->initialize(Config::background_texture_path, graphic_service->getReferenceResolution().x, graphic_service->getReferenceResolution().y, sf::Vector2f(0, 0));
            background_image->setImageAlpha(background_alpha);
        }

        void MainMenuUIController::initializeButtons()
        {
            bubble_sort_button->initialize("Bubble Sort Button", Config::bubble_sort_button_texture_path, button_width, button_height, sf::Vector2f(first_column_button_x_position, bubble_sort_button_y_position));
            insertion_sort_button->initialize("Insertion Sort Button", Config::insertion_sort_button_texture_path, button_width, button_height, sf::Vector2f(first_column_button_x_position, insertion_sort_button_y_position));
            selection_sort_button->initialize("Selection Sort Button", Config::selection_sort_button_texture_path, button_width, button_height, sf::Vector2f(first_column_button_x_position, selection_sort_button_y_position));
            merge_sort_button->initialize("Merge Sort Button", Config::merge_sort_button_texture_path, button_width, button_height, sf::Vector2f(second_column_button_x_position, merge_sort_button_y_position));
            quick_sort_button->initialize("Quick Sort Button", Config::quick_sort_button_texture_path, button_width, button_height, sf::Vector2f(second_column_button_x_position, quick_sort_button_y_position));
            radix_sort_button->initialize("Radix Sort Button", Config::radix_sort_button_texture_path, button_width, button_height, sf::Vector2f(second_column_button_x_position, radix_sort_button_y_position));
            quit_button->initialize("Quit Button", Config::quit_button_texture_path, button_width, button_height, sf::Vector2f(0, quit_button_y_position));

            quit_button->setCentreAlinged();
        }

        void MainMenuUIController::registerButtonCallback()
        {
            bubble_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::bubbleSortButtonCallback, this));
            //insertion_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::insertionSortButtonCallback, this));
            //selection_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::selectionSortButtonCallback, this));
            //merge_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::mergeSortButtonCallback, this));
            //quick_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::quickSortButtonCallback, this));
            //radix_sort_button->registerCallbackFuntion(std::bind(&MainMenuUIController::radixSortButtonCallback, this));
            quit_button->registerCallbackFuntion(std::bind(&MainMenuUIController::quitButtonCallback, this));
        }

        void MainMenuUIController::bubbleSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::insertionSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::selectionSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::mergeSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::quickSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::radixSortButtonCallback()
        {
            ServiceLocator::getInstance()->getSoundService()->playSound(SoundType::BUTTON_CLICK);
        }

        void MainMenuUIController::quitButtonCallback()
        {
            ServiceLocator::getInstance()->getGraphicService()->getGameWindow()->close();
        }

        void MainMenuUIController::update()
        {
            background_image->update();
            bubble_sort_button->update();
            insertion_sort_button->update();
            selection_sort_button->update();
            merge_sort_button->update();
            quick_sort_button->update();
            radix_sort_button->update();
            quit_button->update();
        }

        void MainMenuUIController::render()
        {
            background_image->render();
            bubble_sort_button->render();
            insertion_sort_button->render();
            selection_sort_button->render();
            merge_sort_button->render();
            quick_sort_button->render();
            radix_sort_button->render();
            quit_button->render();
        }

        void MainMenuUIController::show()
        {
            background_image->show();
            bubble_sort_button->show();
            insertion_sort_button->show();
            selection_sort_button->show();
            merge_sort_button->show();
            quick_sort_button->show();
            radix_sort_button->show();
            quit_button->show();
        }

        void MainMenuUIController::destroy()
        {
            delete (bubble_sort_button);
            delete (insertion_sort_button);
            delete (selection_sort_button);
            delete (merge_sort_button);
            delete (quick_sort_button);
            delete (radix_sort_button);
            delete (quit_button);
            delete (background_image);
        }
    }
}
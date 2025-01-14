#pragma once
#include <SFML/Graphics.hpp>
#include "UI/Interface/IUIController.h"
#include "UI/UIElement/ImageView.h"
#include "UI/UIElement/ButtonView.h"

namespace UI
{
	namespace MainMenu
	{
		class MainMenuUIController : public Interface::IUIController
		{
		private:

			// Button Dimensions:
			const float button_width = 360.f;
			const float button_height = 120.f;

			// Button Offsets:
			const float first_column_button_x_position = 420.f;
			const float second_column_button_x_position = 1140.f;

			const float bubble_sort_button_y_position = 200.f;
			const float insertion_sort_button_y_position = 400.f;
			const float selection_sort_button_y_position = 600.f;
			const float merge_sort_button_y_position = 200.f;
			const float quick_sort_button_y_position = 400.f;
			const float radix_sort_button_y_position = 600.f;
			const float quit_button_y_position = 830.f;

			// References:
			UIElement::ImageView* background_image;
			UIElement::ButtonView* bubble_sort_button;
			UIElement::ButtonView* insertion_sort_button;
			UIElement::ButtonView* selection_sort_button;
			UIElement::ButtonView* merge_sort_button;
			UIElement::ButtonView* quick_sort_button;
			UIElement::ButtonView* radix_sort_button;
			UIElement::ButtonView* quit_button;

			const float background_alpha = 85.f;

			void createImage();
			void createButtons();
			void initializeBackgroundImage();
			void initializeButtons();
			void registerButtonCallback();

			void bubbleSortButtonCallback();
			void insertionSortButtonCallback();
			void selectionSortButtonCallback();
			void mergeSortButtonCallback();
			void quickSortButtonCallback();
			void radixSortButtonCallback();
			void quitButtonCallback();

			void changeGameStateToGameplay();

			void destroy();

		public:
			MainMenuUIController();
			~MainMenuUIController();

			void initialize() override;
			void update() override;
			void render() override;
			void show() override;
		};
	}
}
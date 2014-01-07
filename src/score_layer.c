#include "score_layer.h"


static GBitmap* digit_bitmaps[11];
static GBitmap* rotated_digit_bitmaps[11];


struct ScoreLayer {
  Layer* background_layer;

  BitmapLayer** digit_layers;
  uint8_t num_digits;

  int score;

  ScoreLayerOrientation orientation;
};


static void load_resources() {
  digit_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);

  rotated_digit_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
}


static void score_layer_clear(ScoreLayer* score_layer) {
  layer_remove_child_layers(score_layer->background_layer);

  for (int i = 0; i < score_layer->num_digits; ++i)
  {
    if (score_layer->digit_layers[i] != NULL) {
      bitmap_layer_destroy(score_layer->digit_layers[i]);
      score_layer->digit_layers[i] = NULL;
    }
  }
}


static void score_layer_update(ScoreLayer* score_layer) {
  uint8_t* digits = (uint8_t*) malloc(sizeof(uint8_t) * score_layer->num_digits);
  uint8_t num_digits_needed = 0;
  int score = score_layer->score;

  // collect the single digits in score
  do {
    digits[num_digits_needed++] = score % 10;
    score /= 10;
  } while (score != 0 && num_digits_needed < score_layer->num_digits);

  // handle negative values
  if (score_layer->score < 0) {
    // override last digit if it was required
    if (num_digits_needed == score_layer->num_digits) --num_digits_needed;
    digits[num_digits_needed++] = 10; // index of minus sign bitmap
  }

  // clear old score
  score_layer_clear(score_layer);

  // create and layout the digit layers dependent on the orientation
  for (uint8_t i = 0; i < num_digits_needed; ++i)
  {
    // pick the right bitmap for the current orientation
    GBitmap* digit_bitmap = score_layer->orientation == ScoreLayerOrientationNormal ?
                            digit_bitmaps[digits[i]] : rotated_digit_bitmaps[digits[i]];
    GRect bitmap_bounds = digit_bitmap->bounds;
    BitmapLayer* digit_layer = bitmap_layer_create(bitmap_bounds);
    bitmap_layer_set_bitmap(digit_layer, digit_bitmap);
    
    // place according to index in background layer
    GRect background_layer_frame = layer_get_frame(score_layer->background_layer);
    int margin = (background_layer_frame.size.w - num_digits_needed * bitmap_bounds.size.w) / 2;
    if (margin < 0) margin = 0; // digits will not fit into layer...
    // reverse direction for normal orientation since digits are reversed in digits array
    int index = score_layer->orientation == ScoreLayerOrientationNormal ? (num_digits_needed - 1) - i : i;
    bitmap_bounds.origin = (GPoint) {
      .x = margin + index * bitmap_bounds.size.w,
      .y = 0,
    };
    layer_set_frame((Layer*) digit_layer, bitmap_bounds);

    // add to background layer
    layer_add_child(score_layer->background_layer, (Layer*) digit_layer);

    // remember for later
    score_layer->digit_layers[i] = digit_layer;
  }

  free(digits);
}


ScoreLayer* score_layer_create(GRect frame, uint8_t num_digits) {
  if (digit_bitmaps[0] == NULL) load_resources();

  ScoreLayer* score_layer = (ScoreLayer*) malloc(sizeof(ScoreLayer));

  // init fields
  score_layer->num_digits = num_digits;
  score_layer->orientation = ScoreLayerOrientationNormal;
  score_layer->score = 20;

  // init background layer
  // GSize digit_size = digit_bitmaps[0]->bounds.size;
  // GRect background_frame = (GRect) {
  //   .origin = (GPoint) {.x = 0, .y = 0},
  //   .size = (GSize) {.h = digit_size.h, .w = digit_size.w * num_digits}
  // };
  score_layer->background_layer = layer_create(frame);

  // init space for digit layers
  score_layer->digit_layers = (BitmapLayer**) malloc(sizeof(BitmapLayer*) * num_digits);

  score_layer_update(score_layer);

  return score_layer;
}

void score_layer_destroy(ScoreLayer* score_layer) {
  //TOOD
  score_layer_clear(score_layer);

  free(score_layer);
}


Layer* score_layer_get_layer(ScoreLayer* score_layer) {
  return score_layer->background_layer;
}


int score_layer_get_score(ScoreLayer* score_layer) {
  return score_layer->score;
}

void score_layer_set_score(ScoreLayer* score_layer, int score) {
  if (score_layer->score == score) return;

  score_layer->score = score;
  score_layer_update(score_layer);
}


ScoreLayerOrientation score_layer_get_orientation(ScoreLayer* score_layer) {
  return score_layer->orientation;
}


void score_layer_set_orientation(ScoreLayer* score_layer, ScoreLayerOrientation orientation) {
  if (score_layer->orientation == orientation) return;

  score_layer->orientation = orientation;
  score_layer_update(score_layer);
}

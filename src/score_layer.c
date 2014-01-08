#include "score_layer.h"


static GBitmap* digit_bitmaps[11];
static GBitmap* rotated_digit_bitmaps[11];


struct ScoreLayer {
  Layer* background_layer;

  GCornerMask corner_mask;

  BitmapLayer** digit_layers;
  uint8_t num_digits;

  int score;

  ScoreLayerOrientation orientation;
};


static void load_resources() {
  digit_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_0);
  digit_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_1);
  digit_bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2);
  digit_bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_3);
  digit_bitmaps[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_4);
  digit_bitmaps[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_5);
  digit_bitmaps[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_6);
  digit_bitmaps[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_7);
  digit_bitmaps[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_8);
  digit_bitmaps[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_9);
  digit_bitmaps[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUS_SIGN);

  rotated_digit_bitmaps[0] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_0_ROTATED);
  rotated_digit_bitmaps[1] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_1_ROTATED);
  rotated_digit_bitmaps[2] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_2_ROTATED);
  rotated_digit_bitmaps[3] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_3_ROTATED);
  rotated_digit_bitmaps[4] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_4_ROTATED);
  rotated_digit_bitmaps[5] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_5_ROTATED);
  rotated_digit_bitmaps[6] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_6_ROTATED);
  rotated_digit_bitmaps[7] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_7_ROTATED);
  rotated_digit_bitmaps[8] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_8_ROTATED);
  rotated_digit_bitmaps[9] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_DIGIT_9_ROTATED);
  rotated_digit_bitmaps[10] = gbitmap_create_with_resource(RESOURCE_ID_IMAGE_MINUS_SIGN_ROTATED);
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

  // calculate digits for positive score and add minus sign later
  if (score < 0) score = -score;

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
  // 1. iteration: create bitmap layers and determine required size
  uint32_t digits_width = 0;
  uint32_t digits_height = 0;
  for (uint8_t i = 0; i < num_digits_needed; ++i)
  {
    // pick the right bitmap for the current orientation
    GBitmap* digit_bitmap = score_layer->orientation == ScoreLayerOrientationNormal ?
                            digit_bitmaps[digits[i]] : rotated_digit_bitmaps[digits[i]];

    GRect bitmap_bounds = digit_bitmap->bounds;
    digits_width += bitmap_bounds.size.w;
    if (digits_height == 0) digits_height = bitmap_bounds.size.h;

    BitmapLayer* digit_layer = bitmap_layer_create(bitmap_bounds);
    bitmap_layer_set_bitmap(digit_layer, digit_bitmap);
    
    // remember for later
    score_layer->digit_layers[i] = digit_layer;
  }

  GRect background_layer_frame = layer_get_frame(score_layer->background_layer);
  int margin_x = (background_layer_frame.size.w - digits_width) / 2;
  if (margin_x < 0) margin_x = 0; // digits will not fit into layer...
  int margin_y = (background_layer_frame.size.h - digits_height) / 2;

  // 2. iteration: place according to index, orientation and size in background layer
  uint32_t current_x = 0;
  for (uint8_t i = 0; i < num_digits_needed; ++i) {
    // reverse direction for normal orientation since digits are reversed in digits array
    int index = score_layer->orientation == ScoreLayerOrientationNormal ? (num_digits_needed - 1) - i : i;
    BitmapLayer* layer = score_layer->digit_layers[index];

    GRect frame = layer_get_frame((Layer*) layer);
    frame.origin = (GPoint) {
      .x = margin_x + current_x,
      .y = margin_y,
    };
    layer_set_frame((Layer*) layer, frame);

    current_x += frame.size.w;

    // add to background layer
    layer_add_child(score_layer->background_layer, (Layer*) layer);
  }

  free(digits);
}

static void background_layer_update(Layer* me, GContext* ctx) {
  GRect frame = layer_get_frame(me);
  frame.origin = (GPoint){ .x = 0, .y = 0 };
  ScoreLayer* score_layer = (ScoreLayer*) *((ScoreLayer**) layer_get_data(me));

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, frame, 8, score_layer->corner_mask);
}


ScoreLayer* score_layer_create(GRect frame, uint8_t num_digits) {
  if (digit_bitmaps[0] == NULL) load_resources();

  ScoreLayer* score_layer = (ScoreLayer*) malloc(sizeof(ScoreLayer));

  // init fields
  score_layer->num_digits = num_digits;
  score_layer->orientation = ScoreLayerOrientationNormal;
  score_layer->score = 20;
  score_layer->corner_mask = GCornersAll;

  // init background layer
  score_layer->background_layer = layer_create_with_data(frame, sizeof(ScoreLayer*));
  // keep a reference to the score layer for the update proc
  ScoreLayer** data_pointer = (ScoreLayer**) layer_get_data(score_layer->background_layer);
  *data_pointer = score_layer;
  layer_set_update_proc(score_layer->background_layer, background_layer_update);

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

void score_layer_set_corner_mask(ScoreLayer* score_layer, GCornerMask corner_mask) {
  score_layer->corner_mask = corner_mask;
  layer_mark_dirty(score_layer->background_layer);
}

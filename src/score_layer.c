#include <pebble.h>

#include "score_layer.h"


ScoreLayer* score_layer_life_opponent;
ScoreLayer* score_layer_life_player;


static GBitmap* digit_bitmaps[11];
static GBitmap* rotated_digit_bitmaps[11];


struct ScoreLayer {
  Layer* layer;

  GCornerMask corner_mask;

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


static void update_score(ScoreLayer* me, GContext* ctx) {
  uint8_t* digits = (uint8_t*) malloc(sizeof(uint8_t) * me->num_digits);
  uint8_t num_digits_needed = 0;
  int score = me->score;

  // calculate digits for positive score and add minus sign later
  if (score < 0) score = -score;

  // collect the single digits in score
  do {
    digits[num_digits_needed++] = score % 10;
    score /= 10;
  } while (score != 0 && num_digits_needed < me->num_digits);

  // handle negative values
  if (me->score < 0) {
    // override last digit if it was required
    if (num_digits_needed == me->num_digits) --num_digits_needed;
    digits[num_digits_needed++] = 10; // index of minus sign bitmap
  }

  // create and layout the digits dependent on the orientation
  // 1. iteration: create digit bitmaps and determine required size
  uint32_t digits_width = 0;
  uint32_t digits_height = 0;
  bool underline_digits = true; // check if we need to underline ambiguous ('6' & '9') digits
  GBitmap** bitmaps = (GBitmap**) malloc(num_digits_needed * sizeof(GBitmap*));
  for (uint8_t i = 0; i < num_digits_needed; ++i)
  {
    // pick the right bitmap for the current orientation
    GBitmap* digit_bitmap = me->orientation == ScoreLayerOrientationNormal ? digit_bitmaps[digits[i]] : rotated_digit_bitmaps[digits[i]];

    GRect bitmap_bounds = digit_bitmap->bounds;
    digits_width += bitmap_bounds.size.w;
    if (digits_height == 0) digits_height = bitmap_bounds.size.h;

    if (digits[i] != 6 && digits[i] != 9) {
      // we only need to underline digits in numbers that only contain ambiguous digits
      underline_digits = false;
    }

    bitmaps[i] = digit_bitmap;

  }

  GRect layer_frame = layer_get_frame(me->layer);
  int margin_x = (layer_frame.size.w - digits_width) / 2;
  if (margin_x < 0) margin_x = 0; // digits will not fit into layer...
  int margin_y = (layer_frame.size.h - digits_height) / 2;
  if (me->orientation == ScoreLayerOrientationUpsideDown) {
    // compensate slightly off margin when rotated
    margin_y += 1;
  }

  // 2. iteration: draw according to index, orientation and size on layer
  uint32_t current_x = 0;
  for (uint8_t i = 0; i < num_digits_needed; ++i) {
    // reverse direction for normal orientation since digits are reversed in digits array
    int index = me->orientation == ScoreLayerOrientationNormal ? (num_digits_needed - 1) - i : i;
    GBitmap* bitmap = bitmaps[index];

    GRect frame = bitmap->bounds;
    frame.origin = (GPoint) {
      .x = margin_x + current_x,
      .y = margin_y,
    };

    // draw digit onto layer
    graphics_draw_bitmap_in_rect(ctx, bitmap, frame);

    // underline digit if needed
    if (underline_digits) {
      unsigned short left_kerning = 2; //XXX: the '6' and '9' bitmaps have a left kerning
      unsigned short vertical_margin = 1;
      GPoint start, end;

      if (me->orientation == ScoreLayerOrientationNormal) {
        // draw below
        start = (GPoint) {.x = frame.origin.x + left_kerning, .y = frame.origin.y + frame.size.h + vertical_margin};
        end   = (GPoint) {.x = frame.origin.x + frame.size.w - 1, .y = frame.origin.y + frame.size.h + vertical_margin};
      } else {
        // draw above
        start = (GPoint) {.x = frame.origin.x, .y = frame.origin.y - vertical_margin - 1};
        end   = (GPoint) {.x = frame.origin.x + frame.size.w - left_kerning - 1, .y = frame.origin.y - vertical_margin - 1};
      }

      graphics_draw_line(ctx, start, end);
    }

    current_x += frame.size.w;
  }

  free(digits);
  free(bitmaps);
}

static void update_background(ScoreLayer* me, GContext* ctx) {
  GRect frame = layer_get_frame(me->layer);
  frame.origin = (GPoint){ .x = 0, .y = 0 };

  graphics_context_set_fill_color(ctx, GColorWhite);
  graphics_fill_rect(ctx, frame, 8, me->corner_mask);
}

static void score_layer_update(Layer* me, GContext* ctx) {
  ScoreLayer* score_layer = (ScoreLayer*) *((ScoreLayer**) layer_get_data(me));
  update_background(score_layer, ctx);
  update_score(score_layer, ctx);
}

ScoreLayer* score_layer_create(GRect frame, uint8_t num_digits) {
  if (digit_bitmaps[0] == NULL) load_resources();

  ScoreLayer* score_layer = (ScoreLayer*) malloc(sizeof(ScoreLayer));

  // init fields
  score_layer->num_digits = num_digits;
  score_layer->orientation = ScoreLayerOrientationNormal;
  score_layer->score = 20;
  score_layer->corner_mask = GCornersAll;

  // init layer
  score_layer->layer = layer_create_with_data(frame, sizeof(ScoreLayer*));
  // keep a reference to the score layer for the update proc
  ScoreLayer** data_pointer = (ScoreLayer**) layer_get_data(score_layer->layer);
  *data_pointer = score_layer;
  layer_set_update_proc(score_layer->layer, score_layer_update);

  return score_layer;
}

void score_layer_destroy(ScoreLayer* score_layer) {
  layer_destroy(score_layer->layer);

  free(score_layer);
}


Layer* score_layer_get_layer(ScoreLayer* score_layer) {
  return score_layer->layer;
}


int score_layer_get_score(ScoreLayer* score_layer) {
  return score_layer->score;
}

void score_layer_set_score(ScoreLayer* score_layer, int score) {
  if (score_layer->score == score) return;

  score_layer->score = score;
  layer_mark_dirty(score_layer->layer);
}


ScoreLayerOrientation score_layer_get_orientation(ScoreLayer* score_layer) {
  return score_layer->orientation;
}

void score_layer_set_orientation(ScoreLayer* score_layer, ScoreLayerOrientation orientation) {
  if (score_layer->orientation == orientation) return;

  score_layer->orientation = orientation;
  layer_mark_dirty(score_layer->layer);
}

ScoreLayerOrientation score_layer_get_orientation_all() {
  return score_layer_get_orientation(score_layer_life_opponent);
}

void score_layer_set_orientation_all(ScoreLayerOrientation orientation) {
  score_layer_set_orientation(score_layer_life_opponent, orientation);
  score_layer_set_orientation(score_layer_life_player, orientation);
}


void score_layer_set_corner_mask(ScoreLayer* score_layer, GCornerMask corner_mask) {
  score_layer->corner_mask = corner_mask;
  layer_mark_dirty(score_layer->layer);
}

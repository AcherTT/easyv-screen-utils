import { Inject } from '@nestjs/common';
import { SCREEN_UTILS_PROVIDER } from './screen-utils.constants';

/**
 * Injects Bull's queue instance with the given name
 * @param name queue name
 */
export const InjectScreenUtils = (name?: string): ParameterDecorator =>
    Inject(SCREEN_UTILS_PROVIDER);